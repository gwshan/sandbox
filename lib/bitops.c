/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

unsigned long *bitmap_alloc(unsigned int size)
{
	unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);
	unsigned long *addr;

	addr = malloc(len);
	if (addr)
		memset(addr, 0, len);

	return addr;
}

void bitmap_free(unsigned long *addr)
{
	free(addr);
}

void bitmap_set(unsigned long *addr,
		unsigned long start,
		unsigned long size)
{
	unsigned long offset, mask, end;

	/* Set the leading bits */
	offset = (start & (BITS_PER_LONG - 1));
	if (offset) {
		end = min(offset + size, 64);
		mask = GENMASK(end - 1, offset);
		WRITE_ONCE(addr[start / BITS_PER_LONG],
			   READ_ONCE(addr[start / BITS_PER_LONG]) | mask);

		start = ALIGN(start, BITS_PER_LONG);
		size -= (end - offset);
	}

	/* Set the middle bits */
	mask = GENMASK(BITS_PER_LONG - 1, 0);
	while (size >= BITS_PER_LONG) {
		WRITE_ONCE(addr[start / BITS_PER_LONG],
			   READ_ONCE(addr[start / BITS_PER_LONG]) | mask);

		start += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}

	/* Set the tailing bits */
	if (size) {
		mask = GENMASK(size - 1, 0);
		WRITE_ONCE(addr[start / BITS_PER_LONG],
			   READ_ONCE(addr[start / BITS_PER_LONG]) | mask);
	}
}

void bitmap_clear(unsigned long *addr,
		  unsigned long start,
		  unsigned long size)
{
	unsigned long offset, mask, end;

	/* Clear the leading bits */
	offset = (start & (BITS_PER_LONG - 1));
	if (offset) {
		end = min(offset + size, 64);
		mask = GENMASK(end - 1, offset);
		WRITE_ONCE(addr[start / BITS_PER_LONG],
			   READ_ONCE(addr[start / BITS_PER_LONG]) & ~mask);

		start = ALIGN(start, BITS_PER_LONG);
		size -= (end - offset);
	}

	/* Clear the middle bits */
	mask = GENMASK(BITS_PER_LONG - 1, 0);
	while (size >= BITS_PER_LONG) {
		WRITE_ONCE(addr[start / BITS_PER_LONG],
			   READ_ONCE(addr[start / BITS_PER_LONG]) & ~ mask);

		start += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}

	/* Clear the tailing bits */
	if (size) {
		mask = GENMASK(size - 1, 0);
		WRITE_ONCE(addr[start / BITS_PER_LONG],
			   READ_ONCE(addr[start / BITS_PER_LONG]) | mask);
	}
}

unsigned long bitmap_first_zero_bit(unsigned long *addr,
				    unsigned long size)
{
	unsigned long idx, val;

	for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
		val = READ_ONCE(addr[idx]);
		if (val != ~0UL)
			return min(idx * BITS_PER_LONG + ffz(val), size);
	}

	return size;
}

unsigned long bitmap_next_zero_bit(unsigned long *addr,
				   unsigned long start,
				   unsigned long size)
{
	unsigned long idx, offset, end, val, mask;
	unsigned long sz = size;

	/*
	 * Check the leading bits because @start might be unaligned to
	 * BITS_PER_LONG.
	 */
	offset = start & (BITS_PER_LONG - 1);
	if (offset) {
		end = min(size, 64);
		mask = GENMASK(end - 1, offset);
		val = READ_ONCE(addr[start / BITS_PER_LONG]);
		if ((val & mask) != mask) {
			idx = ALIGN_DOWN(start, BITS_PER_LONG);
			return min(idx + ffz(val | ~mask), sz);
		}

		start = ALIGN(start, BITS_PER_LONG);
		size -= (end - offset);
	}

	while (size >= BITS_PER_LONG) {
		val = READ_ONCE(addr[start / BITS_PER_LONG]);
		if (val != ~0UL)
			return min(start + ffz(val), sz);

		start += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}

	/* Check the tailing bits */
	if (size) {
		mask = GENMASK(size - 1, 0);
		val = READ_ONCE(addr[start / BITS_PER_LONG]);
		if ((val & mask) != mask)
			return min(start + ffz(val | ~mask), sz);
	}

	return sz;
}

unsigned long bitmap_first_set_bit(unsigned long *addr,
				   unsigned long size)
{
	unsigned long idx, val;

	for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
		val = READ_ONCE(addr[idx]);
		if (val != 0UL)
			return min(idx * BITS_PER_LONG + ffs(val), size);
	}

	return size;
}

unsigned long bitmap_next_set_bit(unsigned long *addr,
				  unsigned long start,
				  unsigned long size)
{
	unsigned long idx, offset, end, val, mask;
	unsigned long sz = size;

	/*
	 * Check the leading bits because @start might be unaligned to
	 * BITS_PER_LONG.
	 */
	offset = start & (BITS_PER_LONG - 1);
	if (offset) {
		end = min(size, 64);
		mask = GENMASK(end - 1, offset);
		val = READ_ONCE(addr[start / BITS_PER_LONG]);
		if ((val & mask) != 0UL) {
			idx = ALIGN_DOWN(start, BITS_PER_LONG);
			return min(idx + ffs(val & mask), sz);
		}

		start = ALIGN(start, BITS_PER_LONG);
		size -= (end - offset);
	}

	while (size >= BITS_PER_LONG) {
		val = READ_ONCE(addr[start / BITS_PER_LONG]);
		if (val != 0UL)
			return min(start + ffs(val), sz);

		start += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}

	/* Check the tailing bits */
	if (size) {
		mask = GENMASK(size - 1, 0);
		val = READ_ONCE(addr[start / BITS_PER_LONG]);
		if ((val & mask) != 0UL)
			return min(start + ffz(val & mask), sz);
	}

	return sz;
}
