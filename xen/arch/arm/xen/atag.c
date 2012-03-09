/*
 * atag.c
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <xen/types.h>
#include <xen/errno.h>
#include <asm/atag.h>

#define ATAG_TYPE_CORE    (0x54410001)
#define ATAG_TYPE_MEM     (0x54410002)
#define ATAG_TYPE_INITRD2 (0x54420005)
#define ATAG_TYPE_CMDLINE (0x54410009)
#define ATAG_TYPE_NONE    (0x0)

#define ATAG_NEXT(header) ((struct atag_header *) \
	((vaddr_t) headers + \
	 headers->size * sizeof(u32)))

int atags_valid(struct atag_header *headers)
{
	if (headers->tag == ATAG_TYPE_CORE)
		return 0;
	return -EINVAL;
}

struct atag_header *atag_next(struct atag_header *headers, u32 tag)
{
	if (headers->tag == ATAG_TYPE_NONE)
		return NULL;

	/*
	 * Start search from the next tag, repeated calls
	 * to atag_next can be used to enumerate tags of
	 * a certain type.
	 */
	headers = ATAG_NEXT(headers);

	while (headers->tag != tag &&
	       headers->tag != ATAG_TYPE_NONE)
		headers = ATAG_NEXT(headers);

	return headers->tag == ATAG_TYPE_NONE ? NULL : headers;
}

