/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_SPARSEBIT_H
#define __SANDBOX_SPARSEBIT_H

struct sparsebit_node {
	struct sparsebit_node	*parent;
	struct sparse_node	*left;
	struct sparse_node	*right;
	unsigned long		index;
	unsigned long		num_after;
	unsigned int		mask;
};

struct sparsebit {
	struct sparsebit_node	*root;
	unsigned long		num_set;
};

/* APIs */
struct sparsebit *sparsebit_alloc(void);
void sparsebit_set_num(struct sparsebit *s,
		       unsigned long start, unsigned long num);

#endif /* __SANDBOX_SPARSEBIT_H */

