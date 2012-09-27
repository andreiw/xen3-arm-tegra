/*
 *  Original imgblt from Linux framebuffer support.
 *
 *  Copyright (C) 1999 James Simmons (jsimmons@linux-fbdev.org)
 *
 * Differences from that code is removal of big-endian support and
 * reversed pixel components.
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
#include "fb_private.h"

static const u32 cfb_tab8[] = {
   0x00000000,0xff000000,0x00ff0000,0xffff0000,
   0x0000ff00,0xff00ff00,0x00ffff00,0xffffff00,
   0x000000ff,0xff0000ff,0x00ff00ff,0xffff00ff,
   0x0000ffff,0xff00ffff,0x00ffffff,0xffffffff
};

static const u32 cfb_tab16[] = {
   0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff
};

static const u32 cfb_tab32[] = {
   0x00000000, 0xffffffff
};


static inline void color_imageblit(const struct fb_image *image,
                                   struct fb_info *p, u8 __iomem *dst1,
                                   u32 start_index,
                                   u32 pitch_index)
{
   /* Draw the penguin */
   u32 __iomem *dst, *dst2;
   u32 color = 0, val, shift;
   int i, n, bpp = p->bpp;
   u32 null_bits = 32 - bpp;
   const u8 *src = image->data;

   dst2 = (u32 __iomem *) dst1;
   for (i = image->height; i--; ) {
      n = image->width;
      dst = (u32 __iomem *) dst1;
      shift = 0;
      val = 0;

      if (start_index) {
         u32 start_mask = ~fb_shifted_pixels_mask_u32(start_index);
         val = fb_read(dst) & start_mask;
         shift = start_index;
      }
      while (n--) {
         color = *src;

         val |= color << shift;
         if (shift >= null_bits) {
            fb_write(val, dst++);

            val = (shift == null_bits) ? 0 :
               (color >> 32 - shift);
         }
         shift += bpp;
         shift &= (32 - 1);
         src++;
      }
      if (shift) {
         u32 end_mask = fb_shifted_pixels_mask_u32(shift);

         fb_write((fb_read(dst) & end_mask) | val, dst);
      }
      dst1 += p->line_length;
      if (pitch_index) {
         dst2 += p->line_length;
         dst1 = (u8 __iomem *)((long) dst2 & ~(sizeof(u32) - 1));

         start_index += pitch_index;
         start_index &= 32 - 1;
      }
   }
}


static inline void slow_imageblit(const struct fb_image *image, struct fb_info *p,
                                  u8 __iomem *dst1, u32 fgcolor,
                                  u32 bgcolor,
                                  u32 start_index,
                                  u32 pitch_index)
{
   u32 shift, color = 0, bpp = p->bpp;
   u32 __iomem *dst, *dst2;
   u32 val, pitch = p->line_length;
   u32 null_bits = 32 - bpp;
   u32 spitch = (image->width+7)/8;
   const u8 *src = image->data, *s;
   u32 i, j, l;

   dst2 = (u32 __iomem *) dst1;

   for (i = image->height; i--; ) {
      shift = val = 0;
      l = 8;
      j = image->width;
      dst = (u32 __iomem *) dst1;
      s = src;

      /* write leading bits */
      if (start_index) {
         u32 start_mask = ~fb_shifted_pixels_mask_u32(start_index);
         val = fb_read(dst) & start_mask;
         shift = start_index;
      }

      while (j--) {
         l--;
         color = (*s & (1 << l)) ? fgcolor : bgcolor;
         val |= color << shift;

         /* Did the bitshift spill bits to the next long? */
         if (shift >= null_bits) {
            fb_write(val, dst++);
            val = (shift == null_bits) ? 0 :
               (color >> 32 - shift);
         }
         shift += bpp;
         shift &= (32 - 1);
         if (!l) { l = 8; s++; };
      }

      /* write trailing bits */
      if (shift) {
         u32 end_mask = fb_shifted_pixels_mask_u32(shift);

         fb_write((fb_read(dst) & end_mask) | val, dst);
      }

      dst1 += pitch;
      src += spitch;
      if (pitch_index) {
         dst2 += pitch;
         dst1 = (u8 __iomem *)((long) dst2 & ~(sizeof(u32) - 1));
         start_index += pitch_index;
         start_index &= 32 - 1;
      }

   }
}


/*
 * fast_imageblit - optimized monochrome color expansion
 *
 * Only if:  bpp == 8, 16, or 32
 *           image->width is divisible by pixel/dword (ppw);
 *           line_length is divisible by 4;
 *           beginning and end of a scanline is dword aligned
 */
static inline void fast_imageblit(const struct fb_image *image, struct fb_info *p,
                                  u8 __iomem *dst1, u32 fgcolor,
                                  u32 bgcolor)
{
   u32 fgx = fgcolor, bgx = bgcolor, bpp = p->bpp;
   u32 ppw = 32/bpp, spitch = (image->width + 7)/8;
   u32 bit_mask, end_mask, eorx, shift;
   const char *s = image->data, *src;
   u32 __iomem *dst;
   const u32 *tab = NULL;
   int i, j, k;

   switch (bpp) {
   case 8:
      tab = cfb_tab8;
      break;
   case 16:
      tab = cfb_tab16;
      break;
   case 32:
   default:
      tab = cfb_tab32;
      break;
   }

   for (i = ppw-1; i--; ) {
      fgx <<= bpp;
      bgx <<= bpp;
      fgx |= fgcolor;
      bgx |= bgcolor;
   }

   bit_mask = (1 << ppw) - 1;
   eorx = fgx ^ bgx;
   k = image->width/ppw;

   for (i = image->height; i--; ) {
      dst = (u32 __iomem *) dst1, shift = 8; src = s;

      for (j = k; j--; ) {
         shift -= ppw;
         end_mask = tab[(*src >> shift) & bit_mask];
         fb_write((end_mask & eorx)^bgx, dst++);
         if (!shift) { shift = 8; src++; }
      }
      dst1 += p->line_length;
      s += spitch;
   }
}


void fb_imageblit(struct fb_info *p, const struct fb_image *image)
{
   u32 fgcolor, bgcolor, start_index, bitstart, pitch_index = 0;
   u32 bpl = sizeof(u32), bpp = p->bpp;
   u32 width = image->width;
   u32 dx = image->dx, dy = image->dy;
   u8 __iomem *dst1;

   bitstart = (dy * p->line_length * 8) + (dx * bpp);
   start_index = bitstart & (32 - 1);
   pitch_index = (p->line_length & (bpl - 1)) * 8;

   bitstart /= 8;
   bitstart &= ~(bpl - 1);
   dst1 = p->fbmem + bitstart;

   if (image->depth == 1) {
      fgcolor = image->fg_color;
      bgcolor = image->bg_color;

      if (32 % bpp == 0 && !start_index && !pitch_index &&
          ((width & (32/bpp-1)) == 0) &&
          bpp >= 8 && bpp <= 32) {
         fast_imageblit(image, p, dst1, fgcolor, bgcolor);
      } else {
         slow_imageblit(image, p, dst1, fgcolor, bgcolor,
                        start_index, pitch_index);
      }
   } else {
      BUG_ON(image->depth != p->bpp);
      color_imageblit(image, p, dst1, start_index, pitch_index);
   }
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
