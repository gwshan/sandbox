/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

int kvm_vcpu_create(struct kvm_vm *vm, unsigned long entry_point)
{
	struct kvm_vm_mm *mm = &vm->mm;
	struct kvm_vcpu *vcpu = NULL, *tmp;
	struct kvm_vcpu_init init, preferred;
	struct vm_area *vma;
	unsigned long phys, sctlr_el1 = 0, tcr_el1 = 0;
	unsigned int id = 0;
	int ret = 0;

	/*
	 * Automatically assign vCPU ID, which the existing and maximal
	 * one, plus 1. 0 will be taken if there are no existing vCPUs
	 */
	list_for_each_entry(tmp, &vm->vcpu_list, link) {
		if (tmp->id >= id)
			id = tmp->id + 1;
	}

	/* Alloc vCPU */
	vcpu = malloc(sizeof(*vcpu));
	if (!vcpu) {
		fprintf(stderr, "%s: Unable to alloc vcpu\n", __func__);
		return -ENOMEM;
	}

	memset(vcpu, 0, sizeof(*vcpu));
	vcpu->id = id;
	vcpu->entry_point = entry_point;
	INIT_LIST_HEAD(&vcpu->link);

	/* Alloc stack, whose size is one page */
	vma = mm_vma_alloc(mm->mm, 0, mm->page_size, 0, 0);
	phys = kvm_mm_alloc_phys_pages(vm, 1);
	kvm_mm_map(vm, phys, vma->start, mm->page_size);
	vcpu->stack_base = vma->start;
	vcpu->stack_end  = vma->start + mm->page_size;

	/* Create vCPU */
	vcpu->fd = ioctl(vm->fd, KVM_CREATE_VCPU, id);
	if (vcpu->fd < 0) {
		fprintf(stderr, "%s: Unable to create vcpu (%d)\n",
			__func__, id);
		ret = -ENOMEM;
		goto error;
	}

	/* Map vCPU running state */
	vcpu->state_size = ioctl(vm->fd_dev, KVM_GET_VCPU_MMAP_SIZE, NULL);
	if (vcpu->state_size < sizeof(*vcpu->state)) {
		fprintf(stderr, "%s: Invalid state size (0x%lx)\n",
			__func__, vcpu->state_size);
		ret = -ENOSPC;
		goto error;
	}

	vcpu->state = mmap(NULL, vcpu->state_size, PROT_READ | PROT_WRITE,
			   MAP_SHARED, vcpu->fd, 0);
	if (vcpu->state == MAP_FAILED) {
		fprintf(stderr, "%s: Unable to map running state\n", __func__);
		ret = -ENOMEM;
		goto error;
	}

	/* Reset vCPU */
	ret = ioctl(vm->fd, KVM_ARM_PREFERRED_TARGET, &preferred);
	if (ret) {
		fprintf(stderr, "%s: Unable to get the preferred target (%d)\n",
			__func__, ret);
		goto error;
	}

	memset(&init, 0, sizeof(init));
	init.target = preferred.target;
	ret = ioctl(vcpu->fd, KVM_ARM_VCPU_INIT, &init);
	if (ret) {
		fprintf(stderr, "%s: Unable to initialize vCPU (%d)\n",
			__func__, ret);
		goto error;
	}

	/*
	 * Set vCPU registers. Some of the registers might be write-only.
	 * We're just try our best here. Its return value won't be checked.
	 */
	ret = kvm_vcpu_get_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_SCTLR_EL1),
			       &sctlr_el1);
	ret = kvm_vcpu_get_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_TCR_EL1),
				&tcr_el1);
	if (ret) {
		fprintf(stderr, "%s: Unable to get registers (%d)\n",
			__func__, ret);
		goto error;
	}

	sctlr_el1 |= (1 << 0) | (1 << 2) | (1 << 12);	/* M | C | I    */
	tcr_el1   |= (0 << 14);				/* TG0: 4KB     */
	tcr_el1   |= (1UL << 32);			/* IPS: 36 bits */
	tcr_el1   |= (1 << 8) | (1 << 10) | (3 << 12);
	tcr_el1   |= (64 - mm->va_bits);		/* T0SZ         */

	ret = kvm_vcpu_set_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_CPACR_EL1),
			       (3 << 20));
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_SCTLR_EL1),
				sctlr_el1);
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_TCR_EL1),
				tcr_el1);
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_MAIR_EL1),
				(0x00UL)       | (0x04UL << 8)  |
				(0x0cUL << 16) | (0x44UL << 24) |
				(0xffUL << 32) | (0xbbUL << 40));
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_TTBR0_EL1),
				mm->pgtable);
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_SYS_REG(SYS_REG_TPIDR_EL1),
				vcpu->id);
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_CORE_REG(sp_el1),
				vcpu->stack_end);
	ret |= kvm_vcpu_set_reg(vcpu, KVM_ARM64_CORE_REG(regs.pc),
				vcpu->entry_point);

	if (ret) {
		fprintf(stderr, "%s: Unable to set registers (%d)\n",
			__func__, ret);
		goto error;
	}

	/* Add to the list */
	vcpu->vm = vm;
	list_add_tail(&vm->vcpu_list, &vcpu->link);

	return 0;

error:
	if (vcpu && vcpu->state)
		munmap(vcpu->state, vcpu->state_size);
	if (vcpu && vcpu->fd > 0)
		close(vcpu->fd);
	if (vcpu)
		free(vcpu);

	return ret;
}

int kvm_vcpu_get_reg(struct kvm_vcpu *vcpu,
		     unsigned long id,
		     unsigned long *val)
{
	struct kvm_one_reg reg;

	reg.id = id;
	reg.addr = (unsigned long)val;

	return ioctl(vcpu->fd, KVM_GET_ONE_REG, &reg);
}


int kvm_vcpu_set_reg(struct kvm_vcpu *vcpu,
		     unsigned long id,
		     unsigned long val)
{
	struct kvm_one_reg reg;

	reg.id = id;
	reg.addr = (unsigned long)&val;

	return ioctl(vcpu->fd, KVM_SET_ONE_REG, &reg);
}

void kvm_vcpu_destroy(struct kvm_vcpu *vcpu)
{
	list_del(&vcpu->link);
	munmap(vcpu->state, vcpu->state_size);
	close(vcpu->fd);
	free(vcpu);
}
