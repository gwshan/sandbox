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

struct vm_area *mm_find_vma(struct mm *mm,
			    unsigned long addr,
			    struct vm_area **pprev)
{
	struct vm_area *tmp, *vma = NULL;
	struct rb_node *node = mm->root.node;

	while (node) {
		tmp = rb_entry(node, struct vm_area, node);
		if (tmp->end > addr) {
			vma = tmp;
			if (tmp->start <= addr)
				break;

			node = node->left;
		} else {
			node = node->right;
		}
	}

	if (pprev) {
		if (vma) {
			*pprev = vma->prev;
		} else {
			node = rb_last(&mm->root);
			*pprev = node ? rb_entry(node, struct vm_area, node) : NULL;
		}
	}

	return vma;
}

static int find_vma_link(struct mm *mm,
			 unsigned long addr,
			 unsigned long len,
			 struct vm_area **prev,
			 struct rb_node ***link,
			 struct rb_node **parent)
{
	struct vm_area *vma;
	struct rb_node **rb_link, *rb_parent, *rb_prev;

	rb_link = &mm->root.node;
	rb_parent = NULL;
	rb_prev = NULL;

	while (*rb_link) {
		rb_parent = *rb_link;
		vma = rb_entry(rb_parent, struct vm_area, node);
		if (vma->end > addr) {
			if (vma->start < (addr + len))
				return -ENOMEM;

			rb_link = &rb_parent->left;
		} else {
			rb_prev = rb_parent;
			rb_link = &rb_parent->right;
		}
	}

	*parent = rb_parent;
	*link = rb_link;
	*prev = rb_prev ? rb_entry(rb_prev, struct vm_area, node) : NULL;

	return 0;
}

struct vm_area *mm_alloc_vma(struct mm *mm,
			     unsigned long addr,
			     unsigned long len,
			     unsigned long flags,
			     unsigned long prot)
{
	struct vm_area *vma, *prev, *next;
	struct rb_node *node, *parent, **link;
	unsigned long r_start, r_end;

	/*
	 * The address range shouldn't be overlapped with existing areas
	 * if we have fixed address. Otherwise, the RBTree is iterated
	 * from top to down, to search the appropriate gap.
	 */
	if (flags & MM_VMA_FLAG_FIXED) {
		vma = mm_find_vma(mm, addr, &prev);
		if (vma && (addr + len) >= vma->start)
			return NULL;
	} else {
		if (!(addr >= mm->start && (addr + len) < mm->end))
			return NULL;

		/* The toppest range is used if there is no VMAs yet */
		node = rb_last(&mm->root);
		if (!node) {
			addr = mm->end - len;
			goto found;
		}

		/* Search the appropriate gap in the list */
		vma = rb_entry(node, struct vm_area, node);
		while (vma) {
			r_start = vma->end;
			r_end   = (vma->next) ? vma->next->start : mm->end;
			if ((r_end - r_start) >= len) {
				addr = r_end - len;
				goto found;
			}

			vma = vma->prev;
		}

		/* No appropriate gap is found, lets try the leading gap */
		vma = mm->vma;
		r_start = mm->start;
		r_end = vma->start;
		if ((r_end - r_start) >= len) {
			addr = r_end - len;
			goto found;
		}

		return NULL;
	}

found:
	if (find_vma_link(mm, addr, len, &prev, &link, &parent))
		return NULL;

	vma = malloc(sizeof(*vma));
	if (!vma)
		return NULL;

	memset(vma, 0, sizeof(*vma));
	vma->flags = flags;
	vma->start = addr;
	vma->end   = addr + len;
	vma->prot  = prot;

	/* Insert to the list */
	vma->prev = prev;
	if (prev) {
		next = prev->next;
		prev->next = vma;
	} else {
		next = mm->vma;
		mm->vma = vma;
	}

	vma->next = next;
	if (next)
		next->prev = vma;

	/* Inser to the RBTree */
	rb_link_node(&vma->node, parent, link);
	rb_insert(&mm->root, &vma->node);

	return vma;
}
