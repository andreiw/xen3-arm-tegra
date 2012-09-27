/*
 * Original copyarea from Linux framebuffer support.
 *
 * Copyright (C) 2000 James Simmons (jsimmons@linux-fbdev.org)
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
#include <asm/bitops.h>
#include "fb_private.h"

/*
 *  Generic bitwise copy algorithm
 */
static void
bitcpy(struct fb_info *p, unsigned long __iomem *dst, int dst_idx,
       const unsigned long __iomem *src, int src_idx, int bits,
       unsigned n)
{
   unsigned long first, last;
   int const shift = dst_idx-src_idx;
   int left, right;

   first = ~0UL << dst_idx;
   last = ~(~0UL << ((dst_idx+n) % bits));

   if (!shift) {

      /* Same alignment for source and destination. */
      if (dst_idx+n <= bits) {

         /* Single word. */
         if (last) {
            first &= last;
         }
         fb_write(comp(fb_read(src), fb_read(dst), first), dst);
      } else {

         /* Multiple destination words. */

         /* Leading bits. */
         if (first != ~0UL) {
            fb_write(comp(fb_read(src), fb_read(dst), first), dst);
            dst++;
            src++;
            n -= bits - dst_idx;
         }

         /* Main chunk/ */
         n /= bits;
         while (n >= 8) {
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            fb_write(fb_read(src++), dst++);
            n -= 8;
         }
         while (n--) {
            fb_write(fb_read(src++), dst++);
         }


         // Trailing bits
         if (last) {
            fb_write(comp(fb_read(src), fb_read(dst), last), dst);
         }
      }
   } else {

      /* Different alignment for source and dest/ */
      int m;
      unsigned long d0, d1;

      right = shift & (bits - 1);
      left = -shift & (bits - 1);

      if (dst_idx+n <= bits) {

         /* Single destination word. */
         if (last) {
            first &= last;
         }
         d0 = fb_read(src);
         if (shift > 0) {

            /* Single source word. */
            d0 >>= right;
         } else if (src_idx+n <= bits) {

            /* Single source word. */
            d0 <<= left;
         } else {

            /* 2 source words. */
            d1 = fb_read(src + 1);
            d0 = d0<<left | d1>>right;
         }
         fb_write(comp(d0, fb_read(dst), first), dst);
      } else {

         /*
          * Multiple destination words.
          *
          * We must always remember the last value read, because in case
          * SRC and DST overlap bitwise (e.g. when moving just one pixel in
          * 1bpp), we always collect one full long for DST and that might
          * overlap with the current long from SRC. We store this value in
          * 'd0'.
          */
         d0 = fb_read(src++);

         /* Leading bits. */
         if (shift > 0) {

            /* Single source word. */
            d1 = d0;
            d0 >>= right;
            dst++;
            n -= bits - dst_idx;
         } else {

            /* 2 source words/ */
            d1 = fb_read(src++);

            d0 = d0<<left | d1>>right;
            dst++;
            n -= bits - dst_idx;
         }

         fb_write(comp(d0, fb_read(dst), first), dst);
         d0 = d1;

         /* Main chunk. */
         m = n % bits;
         n /= bits;
         while (n >= 4) {
            d1 = fb_read(src++);
            fb_write(d0 << left | d1 >> right, dst++);
            d0 = d1;
            d1 = fb_read(src++);
            fb_write(d0 << left | d1 >> right, dst++);
            d0 = d1;
            d1 = fb_read(src++);
            fb_write(d0 << left | d1 >> right, dst++);
            d0 = d1;
            d1 = fb_read(src++);
            fb_write(d0 << left | d1 >> right, dst++);
            d0 = d1;
            n -= 4;
         }
         while (n--) {
            d1 = fb_read(src++);
            d0 = d0 << left | d1 >> right;
            fb_write(d0, dst++);
            d0 = d1;
         }

         /* Trailing bits. */
         if (last) {
            if (m <= right) {

               /* Single source word. */
               d0 <<= left;
            } else {

               /* 2 source words. */
               d1 = fb_read(src);
               d0 = d0<<left | d1>>right;
            }

            fb_write(comp(d0, fb_read(dst), last), dst);
         }
      }
   }
}

/*
 *  Generic bitwise copy algorithm, operating backward
 */

static void
bitcpy_rev(struct fb_info *p, unsigned long __iomem *dst, int dst_idx,
           const unsigned long __iomem *src, int src_idx, int bits,
           unsigned n)
{
   unsigned long first, last;
   int shift;

   dst += (n-1)/bits;
   src += (n-1)/bits;
   if ((n-1) % bits) {
      dst_idx += (n-1) % bits;
      dst += dst_idx >> (ffs(bits) - 1);
      dst_idx &= bits - 1;
      src_idx += (n-1) % bits;
      src += src_idx >> (ffs(bits) - 1);
      src_idx &= bits - 1;
   }

   shift = dst_idx-src_idx;

   first = ~0UL << bits - 1 - dst_idx;
   last = ~(~0UL << (bits - 1 - ((dst_idx-n) % bits)));

   if (!shift) {

      /* Same alignment for source and destination. */
      if ((unsigned long)dst_idx+1 >= n) {

         /* Single word. */
         if (last) {
            first &= last;
         }
         fb_write( comp( fb_read(src), fb_read(dst), first), dst);
      } else {

         /* Multiple destination words. */

         /* Leading bits. */
         if (first != ~0UL) {
            fb_write( comp( fb_read(src), fb_read(dst), first), dst);
            dst--;
            src--;
            n -= dst_idx+1;
         }

         /* Main chunk. */
         n /= bits;
         while (n >= 8) {
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            fb_write(fb_read(src--), dst--);
            n -= 8;
         }
         while (n--) {
            fb_write(fb_read(src--), dst--);
         }

         /* Trailing bits. */
         if (last) {
            fb_write(comp(fb_read(src), fb_read(dst), last), dst);
         }
      }
   } else {

      /* Different alignment for source and destination. */
      unsigned long d0, d1;
      int m;

      int const left = -shift & (bits-1);
      int const right = shift & (bits-1);

      if ((unsigned long)dst_idx+1 >= n) {

         /* Single destination word. */
         if (last) {
            first &= last;
         }
         d0 = fb_read(src);
         if (shift < 0) {

            /* Single source word. */
            d0 <<= left;
         } else if (1+(unsigned long)src_idx >= n) {

            /* Single source word. */
            d0 >>= right;
         } else {

            /* 2 source words. */
            d1 = fb_read(src - 1);
            d0 = d0>>right | d1<<left;
         }

         fb_write(comp(d0, fb_read(dst), first), dst);
      } else {

         /*
          * Multiple destination words.
          *
          * We must always remember the last value read, because in case
          * SRC and DST overlap bitwise (e.g. when moving just one pixel in
          * 1bpp), we always collect one full long for DST and that might
          * overlap with the current long from SRC. We store this value in
          * 'd0'.
          */
         d0 = fb_read(src--);

         /* Leading bits. */
         if (shift < 0) {

            /* Single source word. */
            d1 = d0;
            d0 <<= left;
         } else {

            /* 2 source words. */
            d1 = fb_read(src--);
            d0 = d0>>right | d1<<left;
         }

         fb_write(comp(d0, fb_read(dst), first), dst);
         d0 = d1;
         dst--;
         n -= dst_idx+1;

         /* Main chunk. */
         m = n % bits;
         n /= bits;
         while (n >= 4) {
            d1 = fb_read(src--);
            fb_write(d0 >> right | d1 << left, dst--);
            d0 = d1;
            d1 = fb_read(src--);
            fb_write(d0 >> right | d1 << left, dst--);
            d0 = d1;
            d1 = fb_read(src--);
            fb_write(d0 >> right | d1 << left, dst--);
            d0 = d1;
            d1 = fb_read(src--);
            fb_write(d0 >> right | d1 << left, dst--);
            d0 = d1;
            n -= 4;
         }
         while (n--) {
            d1 = fb_read(src--);
            d0 = d0 >> right | d1 << left;
            fb_write(d0, dst--);
            d0 = d1;
         }

         /* Trailing bits. */
         if (last) {
            if (m <= left) {

               /* Single source word. */
               d0 >>= right;
            } else {

               /* 2 source words. */
               d1 = fb_read(src);
               d0 = d0>>right | d1<<left;
            }
            fb_write(comp(d0, fb_read(dst), last), dst);
         }
      }
   }
}

void fb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
   u32 dx = area->dx, dy = area->dy, sx = area->sx, sy = area->sy;
   u32 height = area->height, width = area->width;
   unsigned long const bits_per_line = p->line_length*8u;
   unsigned long __iomem *dst = NULL, *src = NULL;
   int bits = BITS_PER_LONG, bytes = bits >> 3;
   int dst_idx = 0, src_idx = 0, rev_copy = 0;

   /*
    * If the beginning of the target area might overlap with the end of
    * the source area, be have to copy the area reverse.
    */
   if ((dy == sy && dx > sx) || (dy > sy)) {
      dy += height;
      sy += height;
      rev_copy = 1;
   }

   /*
    * split the base of the framebuffer into a long-aligned address
    * and the index of the first bit
    */
   dst = src = (unsigned long __iomem *)((unsigned long)p->fbmem &
                                         ~(bytes-1));
   dst_idx = src_idx = 8*((unsigned long)p->fbmem & (bytes-1));

   /* Add offset of source and target area. */
   dst_idx += dy*bits_per_line + dx*p->bpp;
   src_idx += sy*bits_per_line + sx*p->bpp;

   if (rev_copy) {
      while (height--) {
         dst_idx -= bits_per_line;
         src_idx -= bits_per_line;
         dst += dst_idx >> (ffs(bits) - 1);
         dst_idx &= (bytes - 1);
         src += src_idx >> (ffs(bits) - 1);
         src_idx &= (bytes - 1);
         bitcpy_rev(p, dst, dst_idx, src, src_idx, bits,
                    width*p->bpp);
      }
   } else {
      while (height--) {
         dst += dst_idx >> (ffs(bits) - 1);
         dst_idx &= (bytes - 1);
         src += src_idx >> (ffs(bits) - 1);
         src_idx &= (bytes - 1);
         bitcpy(p, dst, dst_idx, src, src_idx, bits,
                width*p->bpp);
         dst_idx += bits_per_line;
         src_idx += bits_per_line;
      }
   }
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
