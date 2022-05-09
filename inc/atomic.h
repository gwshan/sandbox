/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_ATOMIC_H
#define __SANDBOX_ATOMIC_H

#ifdef CONFIG_ARM64
#include "arm64/atomic.h"
#endif

static __always_inline int
atomic_read(int *v)
{
	return arch_atomic_read(v);
}

static __always_inline void
atomic_set(int *v, int i)
{
	arch_atomic_set(v, i);
}

static __always_inline void
atomic_add(int *v, int i)
{
	arch_atomic_add(v, i);
}

static __always_inline int
atomic_add_return(int *v, int i)
{
	return arch_atomic_add_return(v, i);
}

static __always_inline int
atomic_fetch_add(int *v, int i)
{
	return arch_atomic_fetch_add(v, i);
}

static __always_inline void
atomic_sub(int *v, int i)
{
	arch_atomic_sub(v, i);
}

static __always_inline int
atomic_sub_return(int *v, int i)
{
	return arch_atomic_sub_return(v, i);
}

static __always_inline int
atomic_fetch_sub(int *v, int i)
{
	return arch_atomic_fetch_sub(v, i);
}

static __always_inline void
atomic_inc(int *v)
{
	arch_atomic_add(v, 1);
}

static __always_inline int
atomic_inc_return(int *v)
{
	return arch_atomic_add_return(v, 1);
}

static __always_inline int
atomic_fetch_inc(int *v)
{
	return arch_atomic_fetch_add(v, 1);
}

static __always_inline void
atomic_dec(int *v)
{
	arch_atomic_sub(v, 1);
}

static __always_inline int
atomic_dec_return(int *v)
{
	return arch_atomic_sub_return(v, 1);
}

static __always_inline int
atomic_fetch_dec(int *v)
{
	return arch_atomic_fetch_sub(v, 1);
}

static __always_inline void
atomic_and(int *v, int i)
{
	arch_atomic_and(v, i);
}

static __always_inline int
atomic_fetch_and(int *v, int i)
{
	return arch_atomic_fetch_and(v, i);
}

static __always_inline void
atomic_andnot(int *v, int i)
{
	arch_atomic_andnot(v, i);
}

static __always_inline int
atomic_fetch_andnot(int *v, int i)
{
	return arch_atomic_fetch_andnot(v, i);
}

static __always_inline void
atomic_or(int *v, int i)
{
	arch_atomic_or(v, i);
}

static __always_inline int
atomic_fetch_or(int *v, int i)
{
	return arch_atomic_fetch_or(v, i);
}

static __always_inline void
atomic_xor(int *v, int i)
{
	arch_atomic_xor(v, i);
}

static __always_inline int
atomic_fetch_xor(int *v, int i)
{
	return arch_atomic_fetch_xor(v, i);
}

static __always_inline bool
atomic_sub_and_test(int *v, int i)
{
	return arch_atomic_sub_return(v, i) == 0;
}

static __always_inline bool
atomic_dec_and_test(int *v)
{
	return arch_atomic_sub_return(v, 1) == 0;
}

static __always_inline bool
atomic_inc_and_test(int *v)
{
	return arch_atomic_add_return(v, 1) == 0;
}

static __always_inline int64_t
atomic64_read(int64_t *v)
{
	return arch_atomic64_read(v);
}

static __always_inline void
atomic64_set(int64_t *v, int64_t i)
{
	arch_atomic64_set(v, i);
}

static __always_inline void
atomic64_add(int64_t *v, int64_t i)
{
	arch_atomic64_add(v, i);
}

static __always_inline int64_t
atomic64_add_return(int64_t *v, int64_t i)
{
	return arch_atomic64_add_return(v, i);
}

static __always_inline int64_t
atomic64_fetch_add(int64_t *v, int64_t i)
{
	return arch_atomic64_fetch_add(v, i);
}

static __always_inline void
atomic64_sub(int64_t *v, int64_t i)
{
	arch_atomic64_sub(v, i);
}

static __always_inline int64_t
atomic64_sub_return(int64_t *v, int64_t i)
{
	return arch_atomic64_sub_return(v, i);
}

static __always_inline int64_t
atomic64_fetch_sub(int64_t *v, int64_t i)
{
	return arch_atomic64_fetch_sub(v, i);
}

static __always_inline void
atomic64_inc(int64_t *v)
{
	arch_atomic64_add(v, 1);
}

static __always_inline int64_t
atomic64_inc_return(int64_t *v)
{
	return arch_atomic64_add_return(v, 1);
}

static __always_inline int64_t
atomic64_fetch_inc(int64_t *v)
{
	return arch_atomic64_fetch_add(v, 1);
}

static __always_inline void
atomic64_dec(int64_t *v)
{
	arch_atomic64_sub(v, 1);
}

static __always_inline int64_t
atomic64_dec_return(int64_t *v)
{
	return arch_atomic64_sub_return(v, 1);
}

static __always_inline int64_t
atomic64_fetch_dec(int64_t*v)
{
	return arch_atomic64_fetch_sub(v, 1);
}

static __always_inline void
atomic64_and(int64_t *v, int64_t i)
{
	arch_atomic64_and(v, i);
}

static __always_inline int64_t
atomic64_fetch_and(int64_t *v, int64_t i)
{
	return arch_atomic64_fetch_and(v, i);
}

static __always_inline void
atomic64_andnot(int64_t *v, int64_t i)
{
	arch_atomic64_andnot(v, i);
}

static __always_inline int64_t
atomic64_fetch_andnot(int64_t *v, int64_t i)
{
	return arch_atomic64_fetch_andnot(v, i);
}

static __always_inline void
atomic64_or(int64_t *v, int64_t i)
{
	arch_atomic64_or(v, i);
}

static __always_inline int64_t
atomic64_fetch_or(int64_t *v, int64_t i)
{
	return arch_atomic64_fetch_or(v, i);
}

static __always_inline void
atomic64_xor(int64_t *v, int64_t i)
{
	arch_atomic64_xor(v, i);
}

static __always_inline int64_t
atomic64_fetch_xor(int64_t *v, int64_t i)
{
	return arch_atomic64_fetch_xor(v, i);
}

static __always_inline bool
atomic64_sub_and_test(int64_t *v, int64_t i)
{
	return arch_atomic64_sub_return(v, i) == 0;
}

static __always_inline bool
atomic64_dec_and_test(int64_t *v)
{
	return arch_atomic64_sub_return(v, 1) == 0;
}

static __always_inline bool
atomic64_inc_and_test(int64_t *v)
{
	return arch_atomic64_add_return(v, 1) == 0;
}

#endif /* __SANDBOX_ATOMIC_H */

