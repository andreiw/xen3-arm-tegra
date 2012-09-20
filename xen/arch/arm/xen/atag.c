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

#include <xen/kernel.h>
#include <asm/atag.h>

#define ATAG_NEXT(header) ((struct atag_header *) \
                           ((vaddr_t) headers +   \
                            headers->size * sizeof(u32)))

static int atags_check(struct atag_header *headers)
{
   if (headers->tag == ATAG_TYPE_CORE) {
      return 0;
   }

   return -EINVAL;
}

/*
 * Returns a pointer to a tag of desired tag. If headers == NULL,
 * the list of bootloader-passed tags is searched from the start,
 * otherwise the search starts at the tag following the one
 * passed in headers.
 */
struct atag_header *atag_next(struct atag_header *headers, u32 tag)
{
   if (!headers) {
      headers = atag_info_ptr;
      if (atags_check(headers))
         return NULL;
   }

   if (headers->tag == ATAG_TYPE_NONE) {
      return NULL;
   }

   /*
    * Start search from the next tag, repeated calls
    * to atag_next can be used to enumerate tags of
    * a certain type.
    */
   headers = ATAG_NEXT(headers);

   while (headers->tag != tag &&
          headers->tag != ATAG_TYPE_NONE) {
      headers = ATAG_NEXT(headers);
   }

   return headers->tag == ATAG_TYPE_NONE ? NULL : headers;
}

/*
 * Returns the bootloader-passed command line.
 */
char *atag_cmdline(void)
{
   struct atag_cmdline *ac;

   ac = (struct atag_cmdline *) atag_next(NULL, ATAG_TYPE_CMDLINE);
   return ac ? ac->cmdline : NULL;
}

/*
 * Returns the bootloader-passed initrd image.
 */
int atag_initrd(u32 *start, u32 *end)
{
   struct atag_initrd2 *ai;

   ai = (struct atag_initrd2 *) atag_next(NULL, ATAG_TYPE_INITRD2);
   if (!ai) {
      return -ENXIO;
   }

   *start = ai->start;
   *end = *start + ai->size;
   return 0;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
