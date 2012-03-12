/*
 * atag.h
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

#ifndef ARM_ATAG_H
#define ARM_ATAG_H

#define ATAG_TYPE_CORE    (0x54410001)
#define ATAG_TYPE_MEM     (0x54410002)
#define ATAG_TYPE_INITRD2 (0x54420005)
#define ATAG_TYPE_CMDLINE (0x54410009)
#define ATAG_TYPE_NONE    (0x0)

struct atag_header
{
	u32 size;
	u32 tag;
};

struct atag_initrd2
{
	struct atag_header header;
	u32 start;
	u32 size;
};

struct atag_mem
{
	struct atag_header header;
	u32 start;
	u32 size;
};

struct atag_cmdline
{
	struct atag_header header;
	char cmdline[0];
};

char *atag_cmdline(void);
struct atag_header *atag_next(struct atag_header *headers, u32 tag);

extern struct atag_header *atag_info_ptr;

#endif
