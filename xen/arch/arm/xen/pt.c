/*
 * pt.c - page table manipulation code.
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
 *
 */

#include <xen/kernel.h>
#include <asm/config.h>
#include <asm/cpu-ops.h>
#include <asm/page.h>
#include <asm/pt.h>

/*
 * Initializes the boot page tables.
 * 1) Allocate a page directory for the last special megabyte,
 *    which will reference VECTORS_BASE, and PAGE_DIRECTORY_VIRT.
 */
void pt_init(pd_entry_t *initial_page_directory)
{
   
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
