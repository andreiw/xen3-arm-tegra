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
 * These are 32bpp color definitions that have to be converted
 * to hardware color format before use from 0xAABBGGRR.
 */
#define FB_COLOR_BLACK      0x00000000
#define FB_COLOR_DARK_GRAY  0x00222222
#define FB_COLOR_LIGHT_GRAY 0x00999999
#define FB_COLOR_WHITE      0x00FFFFFF
typedef u32 fb_color_t;
typedef u32 hw_color_t;

/*
 * All offsets are from the right,  inside a "pixel" value, which is
 * exactly 'bpp' wide (means: you can use the offset as right argument
 * to <<). A pixel afterwards is a bit stream and is written to video
 * memory as that unmodified.
 */
struct fb_px {
   u32 offset;  /* beginning of bitfield */
   u32 length;  /* length of bitfield */
};

struct fb_info;
typedef int (*fb_ops_init)(struct fb_info *fb);
typedef void (*fb_ops_sync)(struct fb_info *fb);

struct fb_ops {
   fb_ops_init init;
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
   u32        dx;
   u32        dy;
   u32        width;
   u32        height;
   hw_color_t color;
#define FB_ROP_COPY 0
#define FB_ROP_XOR  1
   u8         rop;
};

struct fb_image {
   u32        dx;       /* Where to place image */
   u32        dy;
   u32        width;    /* Size of image */
   u32        height;
   hw_color_t fg_color; /* Only used when a mono bitmap */
   hw_color_t bg_color;
   u8         depth;    /* Depth of the image */
   void       *data;    /* Pointer to image data */
};

struct fb_copyarea {
   u32 dx;
   u32 dy;
   u32 width;
   u32 height;
   u32 sx;
   u32 sy;
};

void fb_register(struct fb_info *fb_info);
void fb_fillrect(struct fb_info *fb_info, const struct fb_fillrect *rect);
void fb_imageblit(struct fb_info *fb_info, const struct fb_image *image);
void fb_copyarea(struct fb_info *fb_info, const struct fb_copyarea *area);
hw_color_t fb_get_color(struct fb_info *fb_info, fb_color_t color);

#endif

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
