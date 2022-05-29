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
