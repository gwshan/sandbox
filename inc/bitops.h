/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_BITOPS_H
#define __SANDBOX_BITOPS_H

#ifdef CONFIG_ARM64
#include "arm64/bitops.h"
#endif

#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)

#define GENMASK(h, l)					\
	((~0UL - (1UL << (l)) + 1) &			\
	 (~0UL >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK_ULL(h, l)				\
	(((~0ULL) - (1ULL << (l)) + 1) &		\
	 (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

#define ffs(x)	__ffs((x))
#define ffz(x)	__ffs(~(x))

static __always_inline void
set_bit(unsigned long *p, unsigned int nr)
{
	arch_set_bit(p, nr);
}

static __always_inline void
clear_bit(unsigned long *p, unsigned int nr)
{
	arch_clear_bit(p, nr);
}

static __always_inline void
change_bit(unsigned long *p, unsigned int nr)
{
	arch_change_bit(p, nr);
}

static __always_inline bool
test_and_set_bit(unsigned long *p, unsigned int nr)
{
	return arch_test_and_set_bit(p, nr);
}

static __always_inline bool
test_and_clear_bit(unsigned long *p, unsigned int nr)
{
	return arch_test_and_clear_bit(p, nr);
}

static __always_inline bool
test_and_change_bit(unsigned long *p, unsigned int nr)
{
	return arch_test_and_change_bit(p, nr);
}

static __always_inline unsigned long
__ffs(unsigned long word)
{
	int num = 0;

#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif
	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}

	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}

	if ((word & 0xf) == 0) {
                num += 4;
                word >>= 4;
        }

	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}

	if ((word & 0x1) == 0)
		num += 1;

	return num;
}

static inline void
bitmap_zero(unsigned long *addr, unsigned int size)
{
	unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);

	memset(addr, 0, len);
}

static inline void
bitmap_fill(unsigned long *addr, unsigned int size)
{
	unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);

	memset(addr, 0xff, len);
}

static inline void
bitmap_copy(unsigned long *dst, unsigned long *src, unsigned int size)
{
	unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);

	memcpy(dst, src, len);
}

#define bitmap_for_each_zero_range(b, e, addr, size)			\
	for ((b) = bitmap_next_zero_bit((addr), 0, (size)),		\
	     (e) = bitmap_next_set_bit((addr), (b) + 1, (size));	\
	     (b) < (size);						\
	     (b) = bitmap_next_zero_bit((addr), (e) + 1, (size)),	\
	     (e) = bitmap_next_set_bit((addr), (b) + 1, (size)))


/* APIs */
unsigned long *bitmap_alloc(unsigned int size);
void bitmap_free(unsigned long *addr);
void bitmap_set(unsigned long *addr,
		unsigned long start, unsigned long size);
void bitmap_clear(unsigned long *addr,
		  unsigned long start, unsigned long size);
unsigned long bitmap_first_zero_bit(unsigned long *addr, unsigned long size);
unsigned long bitmap_next_zero_bit(unsigned long *addr, unsigned long start,
				 unsigned long size);
unsigned long bitmap_first_set_bit(unsigned long *addr, unsigned long size);
unsigned long bitmap_next_set_bit(unsigned long *addr, unsigned long start,
				  unsigned long size);

#endif /* __SANDBOX_BITOPS_H */

