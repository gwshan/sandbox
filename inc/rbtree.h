/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_RBTREE_H
#define __SANDBOX_RBTREE_H

#define RB_RED			0
#define RB_BLACK		1

struct rb_node {
	struct rb_node		*left;
	struct rb_node		*right;
	unsigned long		parent_color;
};

struct rb_root {
	struct rb_node		*node;
};

#define rb_entry(ptr, type, member)					\
	container_of(ptr, type, member)
#define rb_empty_root(root)						\
	((root)->node == NULL)
#define rb_empty_node(node)						\
	((node)->parent_color == (unsigned long)node)
#define rb_clear_node(node)						\
	((node)->parent_color = (unsigned long)node)
#define rb_for_each_entry(pos, root, node, member)			\
	for (node = rb_first(root);					\
	     node && (pos = rb_entry(node, typeof(*(pos)), member));	\
	     node = rb_next(node))
#define rb_for_each_entry_reverse(pos, root, node, member)		\
	for (node = rb_last(root);					\
	     node && (pos = rb_entry(node, typeof(*(pos)), member));	\
	     node = rb_prev(node))

static inline void rb_link_node(struct rb_node *node,
				struct rb_node *parent,
				struct rb_node **link)
{
	node->left = NULL;
	node->right = NULL;
	node->parent_color = (unsigned long)parent;

	*link = node;
}

/* APIs */
void rb_insert(struct rb_root *root, struct rb_node *node);
void rb_erase(struct rb_root *root, struct rb_node *parent);
void rb_replace(struct rb_root *root, struct rb_node *old, struct rb_node *new);
struct rb_node *rb_first(struct rb_root *root);
struct rb_node *rb_last(struct rb_root *root);
struct rb_node *rb_next(struct rb_node *node);
struct rb_node *rb_prev(struct rb_node *node);

#endif /* __SANDBOX_RBTREE_H */

