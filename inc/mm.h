/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_MM_H
#define __SANDBOX_MM_H

struct mm;

#define MM_VMA_FLAG_FIXED		(1UL << 0)

/**
 * struct vm_area - Virtual memory area
 *
 * @flags:		Virtual memory area flags.
 * @start:		Start address of the virtual memory area.
 * @end:		End address of the virtual memory area.
 * @prot:		Protol when the memory is mapped through page table.
 * @prev:		The previous virtual memory area in the list.
 * @next:		The next virtual memory area in the list.
 * @mm:			The memory management struct, to which this area
 *			belongs to.
 * @node:		Used to insert the area to the RBTree of the associated
 *			memory management struct.
 */
struct vm_area {
	unsigned long		flags;
	unsigned long		start;
	unsigned long		end;
	unsigned long		prot;

	struct vm_area		*prev;
	struct vm_area		*next;
	struct mm		*mm;
	struct rb_node		node;
};

/**
 * struct mm - Memory management struct
 *
 * @start:		Start of virtual address space
 * @end:		End of virtual address space
 * @root:		Root of RBTree for virtual memory areas.
 * @vma:		List of virtual memory areas.
 */
struct mm {
	unsigned long		start;
	unsigned long		end;

	struct rb_root		root;
	struct vm_area		*vma;
};

/* APIs */
struct mm *mm_create(unsigned long addr, unsigned long end);
void mm_destroy(struct mm *mm);
struct vm_area *mm_find_vma(struct mm *mm, unsigned long addr,
			    struct vm_area **pprev);
struct vm_area *mm_alloc_vma(struct mm *mm, unsigned long addr,
			     unsigned long len, unsigned long flags,
			     unsigned long prot);

#endif /* __SANDBOX_MM_H */

