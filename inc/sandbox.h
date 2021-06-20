/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_H
#define __SANDBOX_H

#define offsetof(TYPE, MEMBER)					\
	((size_t)&((TYPE *)0)->MEMBER)
#define offsetofend(TYPE, MEMBER)				\
	(offsetof(TYPE, MEMBER) + sizeof(((TYPE *)0)->MEMBER))
#define container_of(PTR, TYPE, MEMBER)	({			\
	const typeof(((TYPE *)0)->member) *__PTR = (PTR);	\
	(TYPE *)((char *)__PTR - offsetof(TYPE, MEMBER); })

#endif /* __SANDBOX_H */

