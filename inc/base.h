/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_BASE_H
#define __SANDBOX_BASE_H

#define offsetof(type, member)					\
	((size_t)&((type *)0)->member)
#define offsetofend(type, member)				\
	(offsetof(type, member) + sizeof(((type *)0)->member))
#define container_of(ptr, type, member)	({			\
	const typeof(((type *)0)->member) *__ptr = (ptr);	\
	(type *)((char *)__ptr - offsetof(type, member); })

#define WRITE_ONCE(x, val)					\
	do {							\
		*(volatile typeof(x) *)&(x) = (val);		\
	} while (0)
#define READ_ONCE(x)						\
	(*(const volatile typeof(x) *)&(x))

#endif /* __SANDBOX_BASE_H */

