/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

struct kvm_vm *kvm_vm_create(void)
{
	struct kvm_vm *vm = NULL;
	struct kvm_vm_mm *mm = NULL;
	struct kvm_userspace_memory_region region;
	int ret;

	vm = malloc(sizeof(*vm));
	if (!vm)
		return NULL;

	memset(vm, 0, sizeof(*vm));
	INIT_LIST_HEAD(&vm->vcpu_list);

	vm->fd_dev = open("/dev/kvm", O_RDWR);
	if (vm->fd_dev < 0) {
		fprintf(stderr, "%s: Unable to open </dev/kvm>\n", __func__);
		goto error;
	}

	vm->fd = ioctl(vm->fd_dev, KVM_CREATE_VM, 0);
	if (vm->fd < 0) {
		fprintf(stderr, "%s: Unable to create VM\n", __func__);
		goto error;
	}

	/* Initialize memory management parameters */
	mm = &vm->mm;
	mm->pa_bits = 36;
	mm->va_bits = 48;
	mm->page_size = 0x1000;
	mm->page_shift = 12;
	mm->pgtable_levels = 4;
	mm->phys_page_base = 0UL;
	mm->phys_page_num = 0x200;
	mm->host_virt_addr = MAP_FAILED;
	mm->phys_page_bits = bitmap_alloc(mm->phys_page_num);
	if (mm->phys_page_bits) {
		bitmap_zero(mm->phys_page_bits, mm->phys_page_num);
	} else {
		fprintf(stderr, "%s: Unable to alloc bitmap (0x%lx)\n",
			__func__, mm->phys_page_num);
		goto error;
	}

	/* Alloc PGDs of the page table */
	mm->pgtable = kvm_mm_alloc_phys_pages(vm, 1);

	/* Initialize memory management struct */
	mm->mm = mm_create(0, 1UL << mm->va_bits);
	if (!mm->mm) {
		fprintf(stderr, "%s: Unable to create memory management struct\n",
			__func__);
		goto error;
	}

	/* Setup memory slot */
	mm->host_virt_addr = mmap(NULL, mm->phys_page_num * mm->page_size,
				  PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mm->host_virt_addr == MAP_FAILED) {
		fprintf(stderr, "%s: Unable to map memory (0x%lx)\n",
			__func__, mm->phys_page_num * mm->page_size);
		goto error;
	}

	ret = madvise(mm->host_virt_addr,
		      mm->phys_page_num * mm->page_size,
		      MADV_NOHUGEPAGE);
	if (ret) {
		fprintf(stderr, "%s: Unable to disable THP (%d)\n",
			__func__, ret);
		goto error;
	}

	region.slot = 0;
	region.flags = 0;
	region.guest_phys_addr = mm->phys_page_base << mm->page_shift;
	region.memory_size = mm->phys_page_num * mm->page_size;
	region.userspace_addr = (unsigned long)mm->host_virt_addr;
	ret = ioctl(vm->fd, KVM_SET_USER_MEMORY_REGION, &region);
	if (ret) {
		fprintf(stderr, "%s: Unable to set user memory region (%d)\n",
			__func__, ret);
		goto error;
	}

	return vm;
error:
	if (mm && mm->host_virt_addr != MAP_FAILED)
		munmap(mm->host_virt_addr, mm->phys_page_num * mm->page_size);
	if (mm && mm->mm)
		mm_destroy(mm->mm);
	if (mm && mm->phys_page_bits)
		bitmap_free(mm->phys_page_bits);
	if (vm && vm->fd)
		close(vm->fd);
	if (vm && vm->fd_dev)
		close(vm->fd_dev);
	if (vm)
		free(vm);

	return NULL;
}

int kvm_vcpu_create(struct kvm_vm *vm, unsigned long entry_point)
{
	struct kvm_vm_mm *mm = &vm->mm;
	struct kvm_vcpu *vcpu = NULL, *tmp;
	struct kvm_vcpu_init init, preferred;
	struct vm_area *vma;
	unsigned long phys, sctlr_el1 = 0, tcr_el1 = 0;
	unsigned int id;
	int ret = 0;

	/* Assign vCPU ID */
	id = 0;
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

	/* Alloc stack */
	vma = mm_alloc_vma(mm->mm, 0, mm->page_size, 0, 0);
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

void kvm_vm_destroy(struct kvm_vm *vm)
{
	struct kvm_vcpu *vcpu, *tmp;
	struct kvm_vm_mm *mm = &vm->mm;

	list_for_each_entry_safe(vcpu, tmp, &vm->vcpu_list, link)
		kvm_vcpu_destroy(vcpu);

	mm_destroy(mm->mm);
	munmap(mm->host_virt_addr, mm->phys_page_num * mm->page_size);
	bitmap_free(mm->phys_page_bits);
	close(vm->fd);
	close(vm->fd_dev);
	free(vm);
}
