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
#include <xen/fbcon.h>
#include <asm/io.h>

static struct fb_info *fb_current = NULL;

/*
 * Registers a framebuffer. This must be done during
 * platform initialization time.
 */
void fb_register(struct fb_info *fb_info)
{
   BUG_ON(fb_current != NULL);

   fb_current = fb_info;
}


/*
 * Initializes the framebuffer subsystem.
 */
static int fb_init(void)
{
   if (!fb_current) {
      printk("fb: no platform framebuffer support\n");
      return 0;
   }
   if (fb_current->ops.init == NULL ||
       fb_current->ops.init(fb_current) == 0) {
         fb_current->flags |= FB_FLAGS_ACTIVE;
   }

   printk("fb: %s, %s\n", fb_current->name,
          fb_current->flags & FB_FLAGS_ACTIVE ?
          "active" : "not-active");

   if (fb_current->flags & FB_FLAGS_ACTIVE) {
      fbcon_init(fb_current);
   }
   return 0;
}

__initcall(fb_init);


/*
 * All operations drawing to the framebuffer take a hw_color_t,
 * while generic color defines are all 32bpp.
 */
hw_color_t fb_get_color(struct fb_info *fb_info, fb_color_t color)
{
   hw_color_t hw = 0;

   /* Handle red. */
   hw |= ((color & 0xFF) * ((1 << fb_info->red.length) - 1) / 0xFF) <<
      fb_info->red.offset;
   color >> 8;
   hw |= ((color & 0xFF) * ((1 << fb_info->green.length) - 1) / 0xFF) <<
      fb_info->green.offset;
   color >> 8;
   hw |= ((color & 0xFF) * ((1 << fb_info->blue.length) - 1) / 0xFF) <<
      fb_info->blue.offset;
   color >> 8;
   hw |= ((color & 0xFF) * ((1 << fb_info->alpha.length) - 1) / 0xFF) <<
      fb_info->alpha.offset;

   return hw;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
