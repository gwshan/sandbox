/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_LIST_H
#define __SANDBOX_LIST_H

#include "sandbox.h"

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name)	\
	{ &(name), &(name) }
#define LIST_HEAD(name)		\
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline int list_empty(struct list_head *list)
{
	return list->next == head;
}

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new,
			    struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new,
				 struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev,
			      struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

#define list_entry(ptr, type, member)					\
	container_of(ptr, type, member)
#define list_first_entry(ptr, type, member)				\
	list_entry((ptr)->next, type, member)
#define list_next_entry(pos, member)					\
	list_entry((pos)->member.next, typeof(*(pos)), member)
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
	     n = list_next_entry(pos, member);				\
	     &pos->member != (head);					\
	     pos = n, n = list_next_entry(n, member))

#endif /* __SANDBOX_LIST_H */

