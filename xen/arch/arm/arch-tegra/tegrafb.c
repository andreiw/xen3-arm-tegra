/*
 * tegrafb.c
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <xen/kernel.h>
#include <xen/fb.h>
#include <xen/mm.h>
#include <asm/platform.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/pt.h>
#include <asm/arch/hardware.h>

#define DC_WINBUF_A_START_ADDR (0x800)
#define DC_CMD_STATE_CONTROL   (0x041)
#define DC_CMD_DISPLAY_WINDOW_HEADER (0x042)

#define XRES  1280
#define YRES  800
#define BPP   32

struct tegra_fb {
   void __iomem *base;
};

static inline u32 tegra_fb_read(struct tegra_fb *fb, int offset)
{
   return readl(fb->base + offset * sizeof(u32));
}

static inline void tegra_fb_write(struct tegra_fb *fb, u32 val, int offset)
{
   writel(val, fb->base + offset * sizeof(u32));
}

int tegra_fb_ops_init(struct fb_info *fb_info)
{
   struct page_info *fb;
   struct tegra_fb *tfb = fb_info->private;

   fb = pages_u_alloc(NULL, get_order_from_bytes(fb_info->fblen), 0);
   BUG_ON(fb == NULL);

   BUG_ON(pt_map(TEGRA_FB_VIRT, page_to_phys(fb), TEGRA_FB_SIZE, PTE_ENTRY_IO) != 0);
   fb_info->fbmem = (void __iomem *) TEGRA_FB_VIRT;

   /* Enable. */
   tegra_fb_write(tfb, (1 << 4), DC_CMD_DISPLAY_WINDOW_HEADER);
   tegra_fb_write(tfb, page_to_phys(fb), DC_WINBUF_A_START_ADDR);

   /* Update already activated. */
   if (tegra_fb_read(tfb, DC_CMD_STATE_CONTROL) & 3) {
      return 0;
   }

   /* Refresh regs. */
   tegra_fb_write(tfb, (1 << 8) | (1 << 9), DC_CMD_STATE_CONTROL);
   tegra_fb_write(tfb, (1 << 0) | (1 << 1), DC_CMD_STATE_CONTROL);
   while (tegra_fb_read(tfb, DC_CMD_STATE_CONTROL) & 3);

   return 0;
}

static struct tegra_fb tegra_fb;
static struct fb_info tegra_fb_info = {
   .name =  "TegraFB",
   .fblen = XRES * YRES * BPP / 8,
   .line_length = XRES * BPP / 8,
   .xres = XRES,
   .yres = YRES,
   .bpp = BPP,
   .ops = {
      .init = tegra_fb_ops_init
   },
   .private = &tegra_fb
};

void tegra_fb_init(void)
{
   unsigned fb;

   tegra_fb.base = (void __iomem *) IO_TO_VIRT(TEGRA_DISPLAY_BASE);

   /* BPP = 32 */
   tegra_fb_info.red.offset = 0;
   tegra_fb_info.red.length = 8;
   tegra_fb_info.green.offset = 8;
   tegra_fb_info.green.length = 8;
   tegra_fb_info.blue.offset = 16;
   tegra_fb_info.blue.length = 8;

   fb_register(&tegra_fb_info);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
