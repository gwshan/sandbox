/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "sandbox.h"

struct mm *mm_create(unsigned long addr,
		     unsigned long end)
{
	struct mm *mm;

	mm = malloc(sizeof(*mm));
	if (!mm) {
		fprintf(stderr, "%s: Unable to alloc mm\n", __func__);
		return NULL;
	}

	memset(mm, 0, sizeof(*mm));
	mm->start = addr;
	mm->end = end;
	mm->root.node = NULL;
	mm->vma = NULL;

	return mm;
}

void mm_destroy(struct mm *mm)
{
	struct vm_area *vma;

	while (mm->vma) {
		vma = mm->vma;
		mm->vma = vma->next;
		if (mm->vma)
			mm->vma->prev = NULL;

		rb_erase(&mm->root, &vma->node);
		free(vma);
	}

	free(mm);
}
