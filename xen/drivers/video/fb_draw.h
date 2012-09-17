#ifndef FB_DRAW_H
#define FB_DRAW_H

#include <xen/kernel.h>
#include <xen/fb.h>
#include <asm/io.h>

#if BITS_PER_LONG == 32
#  define fb_write writel
#  define fb_read  readl
#else
#  define fb_write writeq
#  define fb_read  readq
#endif

/*
 *  Compose two values, using a bitmask as decision value
 *  This is equivalent to (a & mask) | (b & ~mask)
 */

static inline unsigned long
comp(unsigned long a, unsigned long b, unsigned long mask)
{
  return ((a ^ b) & mask) ^ b;
}

/*
 *  Create a pattern with the given pixel's color
 */

#if BITS_PER_LONG == 64
static inline unsigned long
pixel_to_pat(u32 bpp, u32 pixel)
{
   switch (bpp) {
   case 1:
      return 0xfffffffffffffffful*pixel;
   case 2:
      return 0x5555555555555555ul*pixel;
   case 4:
      return 0x1111111111111111ul*pixel;
   case 8:
      return 0x0101010101010101ul*pixel;
   case 12:
      return 0x1001001001001001ul*pixel;
   case 16:
      return 0x0001000100010001ul*pixel;
   case 24:
      return 0x0001000001000001ul*pixel;
   case 32:
      return 0x0000000100000001ul*pixel;
   default:
      panic("pixel_to_pat(): unsupported pixelformat\n");
   }
}
#else
static inline unsigned long
pixel_to_pat( u32 bpp, u32 pixel)
{
   switch (bpp) {
   case 1:
      return 0xfffffffful*pixel;
   case 2:
      return 0x55555555ul*pixel;
   case 4:
      return 0x11111111ul*pixel;
   case 8:
      return 0x01010101ul*pixel;
   case 12:
      return 0x01001001ul*pixel;
   case 16:
      return 0x00010001ul*pixel;
   case 24:
      return 0x01000001ul*pixel;
   case 32:
      return 0x00000001ul*pixel;
   default:
      panic("pixel_to_pat(): unsupported pixelformat\n");
   }
}
#endif

#define fb_shifted_pixels_mask_u32(i)  (~(u32)0 << (i))
#define fb_shifted_pixels_mask_long(i) (~0UL << (i))

static inline unsigned long rolx(unsigned long word, unsigned int shift, unsigned int x)
{
   return (word << shift) | (word >> (x - shift));
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */

#endif /* FB_DRAW_H */
