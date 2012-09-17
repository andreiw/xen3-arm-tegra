/*
 * fb.h
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

#ifndef XEN_FB_H
#define XEN_FB_H

/*
 * All offsets are from the right,  inside a "pixel" value, which is
 * exactly 'bpp' wide (means: you can use the offset as right argument
 * to <<). A pixel afterwards is a bit stream and is written to video
 * memory as that unmodified.
 */
struct fb_px {
   u32 offset; /* beginning of bitfield */
   u32 length;  /* length of bitfield */
};

struct fb_info;
typedef void (*fb_ops_init)(struct fb_info *fb);
typedef void (*fb_ops_sync)(struct fb_info *fb);

struct fb_ops {
   fb_ops_init   init;
   fb_ops_init   sync;
};

struct fb_info {
#define FB_FLAGS_ACTIVE (1 << 0) /* Ready to use. */
   unsigned      flags;
   char          *name;
   size_t        fblen;
   void __iomem *fbmem;
   unsigned      line_length;    /* in bytes */

   unsigned      xres;
   unsigned      yres;
   unsigned      bpp;

   struct fb_px  red;
   struct fb_px  green;
   struct fb_px  blue;
   struct fb_px  alpha;

   struct fb_ops ops;
   void          *private;
};

struct fb_fillrect {
   u32 dx;
   u32 dy;
   u32 width;
   u32 height;
   u32 color;
#define ROP_COPY 0
#define ROP_XOR  1
   unsigned rop;
};

void fb_register(struct fb_info *fb_info);
void cfb_fillrect(struct fb_info *p, const struct fb_fillrect *rect);

#endif

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
