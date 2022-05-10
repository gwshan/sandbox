/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_ARM64_BITOPS_H
#define __SANDBOX_ARM64_BITOPS_H

#define BITS_PER_BYTE		8
#define BITS_PER_LONG		64
#define BITS_PER_LONG_LONG	64
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP((nr), 64)
#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)

static __always_inline void
arch_set_bit(unsigned long *p, unsigned int nr)
{
	p += BIT_WORD(nr);
	arch_atomic64_or(p, BIT_MASK(nr));
}

static __always_inline void
arch_clear_bit(unsigned long *p, unsigned int nr)
{
	p += BIT_WORD(nr);
	arch_atomic64_andnot(p, BIT_MASK(nr));
}

static __always_inline void
arch_change_bit(unsigned long *p, unsigned int nr)
{
	p += BIT_WORD(nr);
	arch_atomic64_xor(p, BIT_MASK(nr));
}

static __always_inline int
arch_test_and_set_bit(unsigned long *p, unsigned int nr)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	if (READ_ONCE(*p) & mask)
		return 1;

	old = arch_atomic64_fetch_or(p, mask);
	return !!(old & mask);
}

static __always_inline int
arch_test_and_clear_bit(unsigned long *p, unsigned int nr)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	if (!(READ_ONCE(*p) & mask))
		return 0;

	old = arch_atomic64_fetch_andnot(p, mask);
	return !!(old & mask);
}

static __always_inline int
arch_test_and_change_bit(unsigned long *p, unsigned int nr)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = arch_atomic64_fetch_xor(p, mask);
	return !!(old & mask);
}

#endif /* __SANDBOX_ARM64_BITOPS_H */

