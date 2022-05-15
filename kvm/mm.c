/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

unsigned long kvm_mm_gpa_to_hva(struct kvm_vm *vm, unsigned long gpa)
{
	return ((unsigned long)(vm->mm.host_virt_addr) +
		(gpa - (vm->mm.phys_page_base << vm->mm.page_shift)));
}

unsigned long kvm_mm_alloc_phys_pages(struct kvm_vm *vm,
				      unsigned long npages)
{
	struct kvm_vm_mm *mm = &vm->mm;
	unsigned long start, end, tend;

	bitmap_for_each_zero_range(start, tend,
		mm->phys_page_bits, mm->phys_page_num) {
		end = min(tend, mm->phys_page_num);
		if (end - start >= npages) {
			bitmap_set(mm->phys_page_bits, start, npages);
			return (start << mm->page_shift);
		}
	}

	return 0;
}

static void map_one_page(struct kvm_vm *vm,
			 unsigned long phys,
			 unsigned long virt)
{
	struct kvm_vm_mm *mm = &vm->mm;
	unsigned long mask, shift, index;
	unsigned long level, *pte;

	pte = (unsigned long *)kvm_mm_gpa_to_hva(vm, mm->pgtable);
	for (level = mm->pgtable_levels; level > 0; level--) {
		shift = (level - 1) * (mm->page_shift - 3) + mm->page_shift;
		index = (virt >> shift) & GENMASK(mm->page_shift - 4, 0);
		pte += index;

		/*
		 * For table entries, we need populate it before it
		 * can be accessed. However, the leaf entries can be
		 * accessed directly.
		 */
		if (level > 1) {
			if (!*pte)
				*pte = kvm_mm_alloc_phys_pages(vm, 1) | 3;

			mask = GENMASK(mm->pa_bits - 1, mm->page_shift);
			pte = (unsigned long *)kvm_mm_gpa_to_hva(vm, *pte & mask);
		} else {
			*pte = (phys | (1 << 10) | (4 << 2) | 3);
		}
	}
}

void kvm_mm_map(struct kvm_vm *vm,
		unsigned long phys,
		unsigned long virt,
		unsigned long len)
{
	struct kvm_vm_mm *mm = &vm->mm;
	unsigned long end = virt + len;

	while (virt < end) {
		map_one_page(vm, phys, virt);

		phys += mm->page_size;
		virt += mm->page_size;
		len -= mm->page_size;
	}
}

