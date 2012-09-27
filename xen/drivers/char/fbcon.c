/*
 * fbcon.c
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
#include <xen/console.h>

static struct fbcon_private {
   struct fb_info *fb;
   struct fbcon_font *font;

   /*
    * Offsets calculated so the white space around the border
    * looks equivalent.
    */
   u8              x_offset;
   u8              y_offset;

   /* Dimensions. */
   unsigned        lines;
   unsigned        cols;

   /* Next position to place char. */
   unsigned        next_line;
   unsigned        next_col;

   /*
    * Colors used.
    */
   hw_color_t      bg_color;
   hw_color_t      fg_color;
} fbcon_private;


static void fbcon_newline(struct fbcon_private *private)
{
   struct fb_copyarea area;
   struct fb_fillrect rect;

   private->next_line++;
   private->next_col = 0;

   if (private->next_line >= private->lines) {

      /*
       * Scroll upwards by a line.
       *
       * Yes, not from x-offset, but it makes the width
       * calculation simpler, no?
       */
      area.dx = 0;
      area.dy = private->y_offset;
      area.sx = 0;
      area.sy = private->y_offset + private->font->height;
      area.width = private->fb->xres;
      area.height = private->font->height * (private->next_line - 1);
      fb_copyarea(private->fb, &area);
      private->next_line--;

      /*
       * Clear line.
       */
      rect.dx = 0;
      rect.dy = private->y_offset + private->font->height *
         private->next_line;
      rect.width = private->fb->xres;
      rect.height = private->font->height;
      rect.color = private->bg_color;
      fb_fillrect(private->fb, &rect);
   }
}


static inline unsigned fbcon_font_index(struct fbcon_font *font, char c)
{

   /*
    * The font is stored in bytes, so a 12-bit width occupies two bytes.
    */
   return (ROUND_UP(font->width, 8) / 8) * c * font->height;
}


static void fbcon_putc(struct fbcon_private *private, char c)
{
   struct fb_image image;

   if (c == '\n' || private->next_col >= private->cols) {
      fbcon_newline(private);

      if (c == '\n') {
         return;
      }
   }

   image.width = private->font->width;
   image.height = private->font->height;
   image.dx = private->x_offset + image.width *
      private->next_col;
   image.dy = private->y_offset + image.height *
      private->next_line;
   image.fg_color = private->fg_color;
   image.bg_color = private->bg_color;
   image.depth = 1;
   image.data = private->font->data +
      fbcon_font_index(private->font, c);
   fb_imageblit(private->fb, &image);

   if (++private->next_col > private->cols) {
      fbcon_newline(private);
   }
}


void fbcon_puts(struct console_info *con, const char *s)
{
   int c;
   struct fbcon_private *private = (struct fbcon_private *) con->private;

   while ((c = *s++) != '\0') {
      fbcon_putc(private, c);
   }
}

static struct console_info fbcon_info = {
   .name = "framebuffer console",
   .puts = fbcon_puts,
   .private = &fbcon_private,
};


void fbcon_init(struct fb_info *fb)
{
   struct fb_fillrect rect;
   BUG_ON(fbcon_private.fb != NULL);

   fbcon_private.fb = fb;
   fbcon_private.fg_color = fb_get_color(fb, FB_COLOR_LIGHT_GRAY);
   fbcon_private.bg_color = fb_get_color(fb, FB_COLOR_BLACK);
   fbcon_private.font = &fbcon_font;
   fbcon_private.lines = fb->yres / fbcon_private.font->height;
   fbcon_private.cols = fb->xres / fbcon_private.font->width;
   fbcon_private.y_offset  = (fb->yres - (fbcon_private.lines *
                                          fbcon_private.font->height)) / 2;
   fbcon_private.x_offset  = (fb->xres - (fbcon_private.cols *
                                          fbcon_private.font->width)) / 2;

   printk("framebuffer console: %s, %ux%u (xoff: %u yoff: %u)\n",
          fb->name,
          fbcon_private.cols, fbcon_private.lines,
          fbcon_private.x_offset, fbcon_private.y_offset);
   rect.dx = 0;
   rect.dy = 0;
   rect.width = fb->xres;
   rect.height = fb->yres;
   rect.color = fbcon_private.bg_color;
   rect.rop = FB_ROP_COPY;
   fb_fillrect(fb, &rect);

   fbcon_info.private = &fbcon_private;
   console_register(&fbcon_info);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
