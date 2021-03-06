/*
 *  linux/include/asm-arm/arch-tegra/hardware.h
 *
 *  Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *  Copyright (C) 1999 ARM Limited.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <asm/arch/tegra-regs.h>

#define TEGRA_RAM_PHYS     0x0
#define TEGRA_RAM_VIRT     0
#define TEGRA_RAM_SIZE     SZ_1G

#define TEGRA_CPU_PHYS     0x50000000
#define TEGRA_CPU_SIZE     SZ_1M
#define TEGRA_CPU_VIRT     IOREMAP_VIRT_START

#define TEGRA_HOST1X_PHYS  0x54000000
#define TEGRA_HOST1X_SIZE  SZ_4M
#define TEGRA_HOST1X_VIRT  (TEGRA_CPU_VIRT + TEGRA_CPU_SIZE)

#define TEGRA_PPSB_PHYS    0x60000000
#define TEGRA_PPSB_SIZE    SZ_1M
#define TEGRA_PPSB_VIRT    (TEGRA_HOST1X_PHYS + TEGRA_HOST1X_SIZE)

#define TEGRA_APB_PHYS     0x70000000
#define TEGRA_APB_SIZE     SZ_1M
#define TEGRA_APB_VIRT     (TEGRA_PPSB_VIRT + TEGRA_PPSB_SIZE)

#define TEGRA_FB_SIZE      SZ_4M
#define TEGRA_FB_VIRT      (TEGRA_APB_VIRT + TEGRA_APB_SIZE)

#define IO_TO_VIRT_BETWEEN(p, st, sz) ((p) >= (st) && (p) < ((st) + (sz)))
#define IO_TO_VIRT_XLATE(p, pst, vst) (((p) - (pst) + (vst)))

#define IO_TO_VIRT(n) ( \
      IO_TO_VIRT_BETWEEN((n), TEGRA_PPSB_PHYS, TEGRA_PPSB_SIZE) ?       \
      IO_TO_VIRT_XLATE((n), TEGRA_PPSB_PHYS, TEGRA_PPSB_VIRT) :         \
      IO_TO_VIRT_BETWEEN((n), TEGRA_APB_PHYS, TEGRA_APB_SIZE) ?         \
      IO_TO_VIRT_XLATE((n), TEGRA_APB_PHYS, TEGRA_APB_VIRT) :           \
      IO_TO_VIRT_BETWEEN((n), TEGRA_CPU_PHYS, TEGRA_CPU_SIZE) ?         \
      IO_TO_VIRT_XLATE((n), TEGRA_CPU_PHYS, TEGRA_CPU_VIRT) :           \
      IO_TO_VIRT_BETWEEN((n), TEGRA_HOST1X_PHYS, TEGRA_HOST1X_SIZE) ?   \
      IO_TO_VIRT_XLATE((n), TEGRA_HOST1X_PHYS, TEGRA_HOST1X_VIRT) :      \
      0)

#endif
