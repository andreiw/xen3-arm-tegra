/*
 * Original fillrect from Linux framebuffer support.
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
 *  Aligned pattern fill using 32/64-bit memory accesses
 */

static void
bitfill_aligned(struct fb_info *p, unsigned long __iomem *dst, int dst_idx,
                unsigned long pat, unsigned n, int bits)
{
   unsigned long first, last;

   if (!n)
      return;

   first = fb_shifted_pixels_mask_long(dst_idx);
   last = ~fb_shifted_pixels_mask_long((dst_idx+n) % bits);

   if (dst_idx+n <= bits) {

      /* Single word */
      if (last)
         first &= last;
      fb_write(comp(pat, fb_read(dst), first), dst);
   } else {
      /* Multiple destination words. */

      /* Leading bits. */
      if (first!= ~0UL) {
         fb_write(comp(pat, fb_read(dst), first), dst);
         dst++;
         n -= bits - dst_idx;
      }

      /* Main chunk. */
      n /= bits;
      while (n >= 8) {
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         fb_write(pat, dst++);
         n -= 8;
      }
      while (n--)
         fb_write(pat, dst++);

      /* Trailing bits. */
      if (last)
         fb_write(comp(pat, fb_read(dst), last), dst);
   }
}


/*
 *  Unaligned generic pattern fill using 32/64-bit memory accesses
 *  The pattern must have been expanded to a full 32/64-bit value
 *  Left/right are the appropriate shifts to convert to the pattern to be
 *  used for the next 32/64-bit word
 */

static void
bitfill_unaligned(struct fb_info *p, unsigned long __iomem *dst, int dst_idx,
                  unsigned long pat, int left, int right, unsigned n, int bits)
{
   unsigned long first, last;

   if (!n)
      return;

   first = ~0UL << dst_idx;
   last = ~(~0UL << ((dst_idx+n) % bits));

   if (dst_idx+n <= bits) {

      /* Single word. */
      if (last)
         first &= last;
      fb_write(comp(pat, fb_read(dst), first), dst);
   } else {

      /*
       * Multiple destination words.
       * Leading bits
       */
      if (first) {
         fb_write(comp(pat, fb_read(dst), first), dst);
         dst++;
         pat = pat << left | pat >> right;
         n -= bits - dst_idx;
      }

      /* Main chunk. */
      n /= bits;
      while (n >= 4) {
         fb_write(pat, dst++);
         pat = pat << left | pat >> right;
         fb_write(pat, dst++);
         pat = pat << left | pat >> right;
         fb_write(pat, dst++);
         pat = pat << left | pat >> right;
         fb_write(pat, dst++);
         pat = pat << left | pat >> right;
         n -= 4;
      }
      while (n--) {
         fb_write(pat, dst++);
         pat = pat << left | pat >> right;
      }

      /* Trailing bits. */
      if (last)
         fb_write(comp(pat, fb_read(dst), last), dst);
   }
}

/*
 *  Aligned pattern invert using 32/64-bit memory accesses
 */

static void
bitfill_aligned_rev(struct fb_info *p, unsigned long __iomem *dst,
                    int dst_idx, unsigned long pat, unsigned n, int bits)
{
   unsigned long val = pat, dat;
   unsigned long first, last;

   if (!n)
      return;

   first = fb_shifted_pixels_mask_long(dst_idx);
   last = ~fb_shifted_pixels_mask_long((dst_idx+n) % bits);

   if (dst_idx+n <= bits) {
      /* Single word. */
      if (last)
         first &= last;
      dat = fb_read(dst);
      fb_write(comp(dat ^ val, dat, first), dst);
   } else {
      /*
       * Multiple destination words.
       */
      if (first!=0UL) {
         dat = fb_read(dst);
         fb_write(comp(dat ^ val, dat, first), dst);
         dst++;
         n -= bits - dst_idx;
      }

      /* Main chunk. */
      n /= bits;
      while (n >= 8) {
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
         n -= 8;
      }
      while (n--) {
         fb_write(fb_read(dst) ^ val, dst);
         dst++;
      }

      /* Trailing bits. */
      if (last) {
         dat = fb_read(dst);
         fb_write(comp(dat ^ val, dat, last), dst);
      }
   }
}


/*
 *  Unaligned generic pattern invert using 32/64-bit memory accesses
 *  The pattern must have been expanded to a full 32/64-bit value
 *  Left/right are the appropriate shifts to convert to the pattern to be
 *  used for the next 32/64-bit word
 */

static void
bitfill_unaligned_rev(struct fb_info *p, unsigned long __iomem *dst,
                      int dst_idx, unsigned long pat, int left, int right,
                      unsigned n, int bits)
{
   unsigned long first, last, dat;

   if (!n)
      return;

   first = ~0UL << dst_idx;
   last = ~(~0UL << ((dst_idx+n) % bits));

   if (dst_idx+n <= bits) {

      /* Single word. */
      if (last)
         first &= last;
      dat = fb_read(dst);
      fb_write(comp(dat ^ pat, dat, first), dst);
   } else {

      /*
       * Multiple destination words.
       * Leading bits.
       */
      if (first != 0UL) {
         dat = fb_read(dst);
         fb_write(comp(dat ^ pat, dat, first), dst);
         dst++;
         pat = pat << left | pat >> right;
         n -= bits - dst_idx;
      }

      /* Main chunk. */
      n /= bits;
      while (n >= 4) {
         fb_write(fb_read(dst) ^ pat, dst);
         dst++;
         pat = pat << left | pat >> right;
         fb_write(fb_read(dst) ^ pat, dst);
         dst++;
         pat = pat << left | pat >> right;
         fb_write(fb_read(dst) ^ pat, dst);
         dst++;
         pat = pat << left | pat >> right;
         fb_write(fb_read(dst) ^ pat, dst);
         dst++;
         pat = pat << left | pat >> right;
         n -= 4;
      }
      while (n--) {
         fb_write(fb_read(dst) ^ pat, dst);
         dst++;
         pat = pat << left | pat >> right;
      }

      /* Trailing bits. */
      if (last) {
         dat = fb_read(dst);
         fb_write(comp(dat ^ pat, dat, last), dst);
      }
   }
}

void cfb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
   unsigned long pat, pat2, fg;
   unsigned long width = rect->width, height = rect->height;
   int bits = BITS_PER_LONG, bytes = bits >> 3;
   u32 bpp = p->bpp;
   unsigned long __iomem *dst;
   int dst_idx, left;

   fg = rect->color;

   pat = pixel_to_pat(bpp, fg);

   dst = (unsigned long __iomem *)((unsigned long)p->fbmem & ~(bytes-1));
   dst_idx = ((unsigned long)p->fbmem & (bytes - 1))*8;
   dst_idx += rect->dy*p->line_length*8+rect->dx*bpp;

   /* FIXME For now we support 1-32 bpp only */
   left = bits % bpp;

   if (!left) {
      void (*fill_op32)(struct fb_info *p,
                        unsigned long __iomem *dst, int dst_idx,
                        unsigned long pat, unsigned n, int bits) = NULL;

      switch (rect->rop) {
      case FB_ROP_XOR:
         fill_op32 = bitfill_aligned_rev;
         break;
      case FB_ROP_COPY:
         fill_op32 = bitfill_aligned;
         break;
      default:
         printk("cfb_fillrect(): unknown rop, defaulting to FB_ROP_COPY\n");
         fill_op32 = bitfill_aligned;
         break;
      }
      while (height--) {
         dst += dst_idx >> (ffs(bits) - 1);
         dst_idx &= (bits - 1);
         fill_op32(p, dst, dst_idx, pat, width*bpp, bits);
         dst_idx += p->line_length*8;
      }
   } else {
      int right, r;
      void (*fill_op)(struct fb_info *p, unsigned long __iomem *dst,
                      int dst_idx, unsigned long pat, int left,
                      int right, unsigned n, int bits) = NULL;
      right = left;
      left = bpp - right;
      switch (rect->rop) {
      case FB_ROP_XOR:
         fill_op = bitfill_unaligned_rev;
         break;
      case FB_ROP_COPY:
         fill_op = bitfill_unaligned;
         break;
      default:
         printk("cfb_fillrect(): unknown rop, defaulting to FB_ROP_COPY\n");
         fill_op = bitfill_unaligned;
         break;
      }
      while (height--) {
         dst += dst_idx / bits;
         dst_idx &= (bits - 1);
         r = dst_idx % bpp;

         /* rotate pattern to the correct start position */
         pat2 = rolx(pat, r, bpp);
         fill_op(p, dst, dst_idx, pat2, left, right,
                 width*bpp, bits);
         dst_idx += p->line_length*8;
      }
   }
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
