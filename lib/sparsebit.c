/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FIXME
 */
#include "sandbox.h"

struct sparsebit *sparsebit_alloc(void)
{
	struct sparsebit *s;

	s = malloc(sizeof(*s));
	if (!s)
		return NULL;

	memset(s, 0, sizeof(*s));

	return s;
}

void sparsebit_set_num(struct sparsebit *s,
		       unsigned long start,
		       unsigned long num)
{
	
}
