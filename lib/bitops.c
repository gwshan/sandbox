/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

unsigned long find_first_zero_bit(unsigned long *addr,
				  unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
		if (addr[idx] != ~0UL)
			return min(idx * BITS_PER_LONG + ffz(addr[idx]), size);
	}

        return size;
}

unsigned long find_next_zero_bit(unsigned long *addr,
				 unsigned long start,
				 unsigned long size)
{
	unsigned long word, idx;

	/* The start position may be unaligned */
	if (start % BITS_PER_LONG) {
		word = addr[start / BITS_PER_LONG] |
		       GENMASK(start % BITS_PER_LONG - 1, 0);
		if (word != ~0UL) {
			idx = ALIGN_DOWN(start, BITS_PER_LONG);
			return min(idx + ffz(word), size);
		}
	}

	for (idx = DIV_ROUND_UP(start, BITS_PER_LONG);
	     idx * BITS_PER_LONG < size; idx++) {
		if (addr[idx] != ~0UL)
			return min(idx * BITS_PER_LONG + ffz(addr[idx]), size);
	}

	return size;
}

unsigned long find_first_bit(unsigned long *addr,
			     unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
		if (addr[idx])
			return min(idx * BITS_PER_LONG + ffs(addr[idx]), size);
	}

	return size;
}

unsigned long find_next_bit(unsigned long *addr,
			    unsigned long start,
			    unsigned long size)
{
	unsigned long word, idx;

	/* The start position may be unaligned */
	if (start % BITS_PER_LONG) {
		word = addr[start / BITS_PER_LONG] &
		       ~GENMASK(start % BITS_PER_LONG - 1, 0);
		if (word) {
			idx = ALIGN_DOWN(start, BITS_PER_LONG);
			return min(idx + ffs(word), size);
		}
	}

	for (idx = DIV_ROUND_UP(start, BITS_PER_LONG);
	     idx * BITS_PER_LONG < size; idx++) {
		if (addr[idx])
			return min(idx * BITS_PER_LONG + ffs(addr[idx]), size);
	}

	return size;
}

void bitmap_set(unsigned long *addr,
		unsigned long start,
		unsigned long size)
{
	unsigned long mask, end;

	if (start % BITS_PER_LONG) {
		end = min(start & BITS_PER_LONG + size, 63);
		mask = GENMASK(end, start % BITS_PER_LONG);
		addr[start / BITS_PER_LONG] |= mask;

		start = ALIGN(start, BITS_PER_LONG);
		size -= (BITS_PER_LONG - start % BITS_PER_LONG);
	}

	mask = GENMASK(BITS_PER_LONG - 1, 0);
	while (size >= BITS_PER_LONG) {
		addr[start / BITS_PER_LONG] |= mask;

		start += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}

	if (size) {
		mask = GENMASK(size - 1, 0);
		addr[start / BITS_PER_LONG] |= mask;
	}
}

void bitmap_clear(unsigned long *addr,
		  unsigned long start,
		  unsigned long size)
{
	unsigned long mask, end;

	if (start % BITS_PER_LONG) {
		end = min(start & BITS_PER_LONG + size, 63);
		mask = GENMASK(end, start % BITS_PER_LONG);
		addr[start / BITS_PER_LONG] &= ~mask;

		start = ALIGN(start, BITS_PER_LONG);
		size -= (BITS_PER_LONG - start % BITS_PER_LONG);
	}

	mask = GENMASK(BITS_PER_LONG - 1, 0);
	while (size >= BITS_PER_LONG) {
		addr[start / BITS_PER_LONG] &= ~mask;

		start += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}

	if (size) {
		mask = GENMASK(size - 1, 0);
		addr[start / BITS_PER_LONG] &= ~mask;
	}
}

unsigned long *bitmap_alloc(unsigned int size)
{
	unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);
	unsigned long *addr;

	addr = malloc(len);
	if (addr)
		memset(addr, 0, len);

	return NULL;
}

void bitmap_free(unsigned long *p)
{
	free(p);
}
