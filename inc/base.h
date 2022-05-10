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

/* Compiler related attributes */
#define __stringify(x...)	#x
#define __no_inline		__attribute__((__noinline__))
#define __weak			__attribute__((__weak__))
#define __packed		__attribute__((__packed__))
#define __aligned(x)		__attribute__((__aligned(x)))

#define WRITE_ONCE(x, val)					\
	do {							\
		*(volatile typeof(x) *)&(x) = (val);		\
	} while (0)
#define READ_ONCE(x)						\
	(*(const volatile typeof(x) *)&(x))

/* struct offsets */
#define offsetof(type, member)					\
	((size_t)&((type *)0)->member)
#define offsetofend(type, member)				\
	(offsetof(type, member) + sizeof(((type *)0)->member))
#define container_of(ptr, type, member)	({			\
	const typeof(((type *)0)->member) *__ptr = (ptr);	\
	(type *)((char *)__ptr - offsetof(type, member); })

/* Math */
#define min(x, y)		((x) < (y) ? (x) : (y))
#define max(x, y)		((x) < (y) ? (y) : (x))
#define ALIGN(x, a)		(((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a)	((x) & ((a) - 1))
#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))

#endif /* __SANDBOX_BASE_H */

