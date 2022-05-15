/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

static inline bool rb_is_red(struct rb_node *node)
{
	return (node->parent_color & (RB_BLACK | RB_RED)) == RB_RED;
}

static inline bool rb_is_black(struct rb_node *node)
{
	return (node->parent_color & (RB_BLACK | RB_RED)) == RB_BLACK;
}

static inline void rb_set_black(struct rb_node *node)
{
	node->parent_color |= RB_BLACK;
}

static inline struct rb_node *rb_parent(struct rb_node *node)
{
	return (struct rb_node *)(node->parent_color & ~(RB_BLACK | RB_RED));
}

static inline struct rb_node *rb_red_parent(struct rb_node *node)
{
	return (struct rb_node *)node->parent_color;
}

static inline void rb_set_parent(struct rb_node *node,
				 struct rb_node *parent)
{
	node->parent_color = (unsigned long)parent |
			     (node->parent_color & (RB_BLACK | RB_RED));
}

static inline void rb_set_parent_color(struct rb_node *node,
				       struct rb_node *parent,
				       int color)
{
	node->parent_color = (unsigned long)parent | color;
}

static inline void rb_change_child(struct rb_root *root,
				   struct rb_node *parent,
				   struct rb_node *old,
				   struct rb_node *new)
{
	if (parent) {
		if (parent->left == old)
			WRITE_ONCE(parent->left, new);
		else
			WRITE_ONCE(parent->right, new);
	} else {
		WRITE_ONCE(root->node, new);
	}
}

static inline void rb_rotate_set_parents(struct rb_root *root,
					 struct rb_node *old,
					 struct rb_node *new,
					 int color)
{
	struct rb_node *parent = rb_parent(old);
	new->parent_color = old->parent_color;
	rb_set_parent_color(old, new, color);
	rb_change_child(root, parent, old, new);
}

void rb_insert(struct rb_root *root, struct rb_node *node)
{
	struct rb_node *parent = rb_red_parent(node), *gparent, *tmp;

	while (true) {
		if (!parent) {
			/*
			 * The inserted node is root. Either this is the
			 * first node, or we recursed at Case 1 below and
			 * are no longer violating 4).
			 */
			rb_set_parent_color(node, NULL, RB_BLACK);
			break;
		}

		/*
		 * If there is a black parent, we are done.
		 * Otherwise, take some corrective action as,
		 * per 4), we don't want a red root or two
		 * consecutive red nodes.
		 */
		if(rb_is_black(parent))
			break;

		gparent = rb_red_parent(parent);

		tmp = gparent->right;
		if (parent != tmp) {	/* parent == gparent->left */
			if (tmp && rb_is_red(tmp)) {
				/*
				 * Case 1 - node's uncle is red (color flips).
				 *
				 *       G            g
				 *      / \          / \
				 *     p   u  -->   P   U
				 *    /            /
				 *   n            n
				 *
				 * However, since g's parent might be red, and
				 * 4) does not allow this, we need to recurse
				 * at g.
				 */
				rb_set_parent_color(tmp, gparent, RB_BLACK);
				rb_set_parent_color(parent, gparent, RB_BLACK);
				node = gparent;
				parent = rb_parent(node);
				rb_set_parent_color(node, parent, RB_RED);
				continue;
			}

			tmp = parent->right;
			if (node == tmp) {
				/*
				 * Case 2 - node's uncle is black and node is
				 * the parent's right child (left rotate at parent).
				 *
				 *      G             G
				 *     / \           / \
				 *    p   U  -->    n   U
				 *     \           /
				 *      n         p
				 *
				 * This still leaves us in violation of 4), the
				 * continuation into Case 3 will fix that.
				 */
				tmp = node->left;
				WRITE_ONCE(parent->right, tmp);
				WRITE_ONCE(node->left, parent);
				if (tmp)
					rb_set_parent_color(tmp, parent,
							    RB_BLACK);
				rb_set_parent_color(parent, node, RB_RED);
				parent = node;
				tmp = node->right;
			}

			/*
			 * Case 3 - node's uncle is black and node is
			 * the parent's left child (right rotate at gparent).
			 *
			 *        G           P
			 *       / \         / \
			 *      p   U  -->  n   g
			 *     /                 \
			 *    n                   U
			 */
			WRITE_ONCE(gparent->left, tmp); /* == parent->right */
			WRITE_ONCE(parent->right, gparent);
			if (tmp)
				rb_set_parent_color(tmp, gparent, RB_BLACK);
			rb_rotate_set_parents(root, gparent, parent, RB_RED);
			break;
		} else {
			tmp = gparent->left;
			if (tmp && rb_is_red(tmp)) {
				/* Case 1 - color flips */
				rb_set_parent_color(tmp, gparent, RB_BLACK);
				rb_set_parent_color(parent, gparent, RB_BLACK);
				node = gparent;
				parent = rb_parent(node);
				rb_set_parent_color(node, parent, RB_RED);
				continue;
			}

			tmp = parent->left;
			if (node == tmp) {
				/* Case 2 - right rotate at parent */
				tmp = node->right;
				WRITE_ONCE(parent->left, tmp);
				WRITE_ONCE(node->right, parent);
				if (tmp) {
					rb_set_parent_color(tmp, parent,
							    RB_BLACK);
				}
				rb_set_parent_color(parent, node, RB_RED);
				parent = node;
				tmp = node->left;
			}

			/* Case 3 - left rotate at gparent */
			WRITE_ONCE(gparent->right, tmp); /* == parent->left */
			WRITE_ONCE(parent->left, gparent);
			if (tmp)
				rb_set_parent_color(tmp, gparent, RB_BLACK);
			rb_rotate_set_parents(root, gparent, parent, RB_RED);
			break;
		}
	}
}

static struct rb_node *__rb_erase_filter(struct rb_root *root, struct rb_node *node)
{
	struct rb_node *child = node->right;
	struct rb_node *tmp = node->left;
	struct rb_node *parent, *rebalance;
	unsigned long pc;

	if (!tmp) {
		/*
 	         * Case 1: node to erase has no more than 1 child (easy!)
		 *
	         * Note that if there is one child it must be red due to 5)
	         * and node must be black due to 4). We adjust colors locally
		 * so as to bypass __rb_erase_color() later on.
		 */
		pc = node->parent_color;
		parent = (struct rb_node *)(pc & ~(RB_BLACK | RB_RED));
		rb_change_child(root, parent, node, child);
		if (child) {
			child->parent_color = pc;
			rebalance = NULL;
		} else {
			rebalance = (pc & RB_BLACK) ? parent : NULL;
		}

		tmp = parent;
	} else if (!child) {
		/* Still case 1, but this time the child is node->left */
		tmp->parent_color = pc = node->parent_color;
		parent = (struct rb_node *)(pc & ~(RB_BLACK | RB_RED));
		rb_change_child(root, parent, node, tmp);
		rebalance = NULL;
		tmp = parent;
	} else {
		struct rb_node *successor = child, *child2;

		tmp = child->left;
		if (!tmp) {
			/*
			 * Case 2: node's successor is its right child
			 *
			 *    (n)          (s)
			 *    / \          / \
			 *  (x) (s)  ->  (x) (c)
			 *        \
			 *        (c)
			 */
			parent = successor;
			child2 = successor->right;
		} else {
			/*
			 * Case 3: node's successor is leftmost under
			 * node's right child subtree
			 *
			 *    (n)          (s)
			 *    / \          / \
			 *  (x) (y)  ->  (x) (y)
			 *      /            /
			 *    (p)          (p)
			 *    /            /
			 *  (s)          (c)
			 *    \
			 *    (c)
			 */
			do {
				parent = successor;
				successor = tmp;
				tmp = tmp->left;
			} while (tmp);
			child2 = successor->right;
			WRITE_ONCE(parent->left, child2);
			WRITE_ONCE(successor->right, child);
			rb_set_parent(child, successor);
		}

		tmp = node->left;
		WRITE_ONCE(successor->left, tmp);
		rb_set_parent(tmp, successor);

		pc = node->parent_color;
		tmp = (struct rb_node *)(pc & ~(RB_BLACK | RB_RED));
		rb_change_child(root, tmp, node, successor);

		if (child2) {
			rb_set_parent_color(child2, parent, RB_BLACK);
			rebalance = NULL;
		} else {
			rebalance = rb_is_black(successor) ? parent : NULL;
		}
		successor->parent_color = pc;
		tmp = successor;
	}

	return rebalance;
}

static void __rb_erase(struct rb_root *root, struct rb_node *parent)
{
	struct rb_node *node = NULL, *sibling, *tmp1, *tmp2;

	while (true) {
		/*
		 * Loop invariants:
		 * - node is black (or NULL on first iteration)
		 * - node is not the root (parent is not NULL)
		 * - All leaf paths going through parent and node have a
		 *   black node count that is 1 lower than other leaf paths.
		 */
		sibling = parent->right;
		if (node != sibling) {	/* node == parent->left */
			if (rb_is_red(sibling)) {
				/*
				 * Case 1 - left rotate at parent
				 *
				 *     P               S
				 *    / \             / \
				 *   N   s    -->    p   Sr
				 *      / \         / \
				 *     Sl  Sr      N   Sl
				 */
				tmp1 = sibling->left;
				WRITE_ONCE(parent->right, tmp1);
				WRITE_ONCE(sibling->left, parent);
				rb_set_parent_color(tmp1, parent, RB_BLACK);
				rb_rotate_set_parents(root, parent,
						      sibling, RB_RED);
				sibling = tmp1;
			}

			tmp1 = sibling->right;
			if (!tmp1 || rb_is_black(tmp1)) {
				tmp2 = sibling->left;
				if (!tmp2 || rb_is_black(tmp2)) {
					/*
					 * Case 2 - sibling color flip
					 * (p could be either color here)
					 *
					 *    (p)           (p)
					 *    / \           / \
					 *   N   S    -->  N   s
					 *      / \           / \
					 *     Sl  Sr        Sl  Sr
					 *
					 * This leaves us violating 5) which
					 * can be fixed by flipping p to black
					 * if it was red, or by recursing at p.
					 * p is red when coming from Case 1.
					 */
					rb_set_parent_color(sibling, parent,
							    RB_RED);
					if (rb_is_red(parent))
						rb_set_black(parent);
					else {
						node = parent;
						parent = rb_parent(node);
						if (parent)
							continue;
					}
					break;
				}
				/*
				 * Case 3 - right rotate at sibling
				 * (p could be either color here)
				 *
				 *   (p)           (p)
				 *   / \           / \
				 *  N   S    -->  N   sl
				 *     / \             \
				 *    sl  Sr            S
				 *                       \
				 *                        Sr
				 *
				 * Note: p might be red, and then both
				 * p and sl are red after rotation(which
				 * breaks property 4). This is fixed in
				 * Case 4 (in rb_rotate_set_parents()
				 *         which set sl the color of p
				 *         and set p RB_BLACK)
				 *
				 *   (p)            (sl)
				 *   / \            /  \
				 *  N   sl   -->   P    S
				 *       \        /      \
				 *        S      N        Sr
				 *         \
				 *          Sr
				 */
				tmp1 = tmp2->right;
				WRITE_ONCE(sibling->left, tmp1);
				WRITE_ONCE(tmp2->right, sibling);
				WRITE_ONCE(parent->right, tmp2);
				if (tmp1)
					rb_set_parent_color(tmp1, sibling,
							    RB_BLACK);
				tmp1 = sibling;
				sibling = tmp2;
			}
			/*
			 * Case 4 - left rotate at parent + color flips
			 * (p and sl could be either color here.
			 *  After rotation, p becomes black, s acquires
			 *  p's color, and sl keeps its color)
			 *
			 *      (p)             (s)
			 *      / \             / \
			 *     N   S     -->   P   Sr
			 *        / \         / \
			 *      (sl) sr      N  (sl)
			 */
			tmp2 = sibling->left;
			WRITE_ONCE(parent->right, tmp2);
			WRITE_ONCE(sibling->left, parent);
			rb_set_parent_color(tmp1, sibling, RB_BLACK);
			if (tmp2)
				rb_set_parent(tmp2, parent);
			rb_rotate_set_parents(root, parent, sibling, RB_BLACK);
			break;
		} else {
			sibling = parent->left;
			if (rb_is_red(sibling)) {
				/* Case 1 - right rotate at parent */
				tmp1 = sibling->right;
				WRITE_ONCE(parent->left, tmp1);
				WRITE_ONCE(sibling->right, parent);
				rb_set_parent_color(tmp1, parent, RB_BLACK);
				rb_rotate_set_parents(root, parent,
						      sibling, RB_RED);
				sibling = tmp1;
			}
			tmp1 = sibling->left;
			if (!tmp1 || rb_is_black(tmp1)) {
				tmp2 = sibling->right;
				if (!tmp2 || rb_is_black(tmp2)) {
					/* Case 2 - sibling color flip */
					rb_set_parent_color(sibling, parent,
							    RB_RED);
					if (rb_is_red(parent))
						rb_set_black(parent);
					else {
						node = parent;
						parent = rb_parent(node);
						if (parent)
							continue;
					}
					break;
				}
				/* Case 3 - left rotate at sibling */
				tmp1 = tmp2->left;
				WRITE_ONCE(sibling->right, tmp1);
				WRITE_ONCE(tmp2->left, sibling);
				WRITE_ONCE(parent->left, tmp2);
				if (tmp1)
					rb_set_parent_color(tmp1, sibling,
							    RB_BLACK);
				tmp1 = sibling;
				sibling = tmp2;
			}
			/* Case 4 - right rotate at parent + color flips */
			tmp2 = sibling->right;
			WRITE_ONCE(parent->left, tmp2);
			WRITE_ONCE(sibling->right, parent);
			rb_set_parent_color(tmp1, sibling, RB_BLACK);
			if (tmp2)
				rb_set_parent(tmp2, parent);
			rb_rotate_set_parents(root, parent, sibling, RB_BLACK);
			break;
		}
	}
}

void rb_erase(struct rb_root *root, struct rb_node *node)
{
	struct rb_node *rebalance;

	rebalance = __rb_erase_filter(root, node);
	if (rebalance)
		__rb_erase(root, rebalance);
}

void rb_replace(struct rb_root *root, struct rb_node *old, struct rb_node *new)
{
	struct rb_node *parent = rb_parent(old);

	/* Copy the pointers/colour from the old to the replacement */
	*new = *old;

	/* Set the surrounding nodes to point to the replacement */
	if (old->left)
		rb_set_parent(old->left, new);
	if (old->right)
		rb_set_parent(old->right, new);

	rb_change_child(root, parent, old, new);
}

struct rb_node *rb_first(struct rb_root *root)
{
	struct rb_node *node;

	node = root->node;
	if (!node)
		return NULL;

	while (node->left)
		node = node->left;

	return node;
}

struct rb_node *rb_last(struct rb_root *root)
{
	struct rb_node *node;

	node = root->node;
	if (!node)
		return NULL;

	while (node->right)
		node = node->right;

	return node;
}

struct rb_node *rb_next(struct rb_node *node)
{
	struct rb_node *parent;

	if (rb_empty_node(node))
		return NULL;

	/*
	 * If we have a right-hand child, go down and then left as far
	 * as we can.
	 */
	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;

		return node;
	}

	/*
	 * No right-hand children. Everything down and left is smaller than us,
	 * so any 'next' node must be in the general direction of our parent.
	 * Go up the tree; any time the ancestor is a right-hand child of its
	 * parent, keep going up. First time it's a left-hand child of its
	 * parent, said parent is our 'next' node.
	 */
	while ((parent = rb_parent(node)) && node == parent->right)
		node = parent;

	return parent;
}

struct rb_node *rb_prev(struct rb_node *node)
{
	struct rb_node *parent;

	if (rb_empty_node(node))
		return NULL;

	/*
	 * If we have a left-hand child, go down and then right as far
	 * as we can.
	 */
	if (node->left) {
		node = node->left;
		while (node->right)
			node = node->right;

		return node;
	}

	/*
	 * No left-hand children. Go up till we find an ancestor which
	 * is a right-hand child of its parent.
	 */
	while ((parent = rb_parent(node)) && node == parent->left)
		node = parent;

	return parent;
}
