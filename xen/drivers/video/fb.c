/*
 * fb.c
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
#include <xen/fb.h>
#include <asm/io.h>

static struct fb_info *fb_current = NULL;

void fb_register(struct fb_info *fb_info)
{
   BUG_ON(fb_current != NULL);

   fb_current = fb_info;
}


static int fb_init(void)
{
   if (!fb_current) {
      printk("fb: no framebuffer support\n");
      return 0;
   }

   printk("fb: %s\n", fb_current->name);
   if (fb_current->ops.init) {
      if (!fb_current->ops.init(fb_current)) {
         fb_current->flags |= FB_FLAGS_ACTIVE;
      }
   } else {
      fb_current->flags |= FB_FLAGS_ACTIVE;
   }

   return 0;
}


void fb_fillrect(const struct fb_fillrect *rect)
{
   if (!fb_current ||
       !(fb_current->flags & FB_FLAGS_ACTIVE)) {
      return;
   }

   cfb_fillrect(fb_current, rect);
}


void fb_imageblit(const struct fb_image *image)
{
   if (!fb_current ||
       !(fb_current->flags & FB_FLAGS_ACTIVE)) {
      return;
   }

   cfb_imageblit(fb_current, image);
}

__initcall(fb_init);

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
