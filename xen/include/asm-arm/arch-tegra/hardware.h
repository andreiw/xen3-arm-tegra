/*
 *  linux/include/asm-arm/arch-imx/hardware.h
 *
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

#include <asm/sizes.h>
#include <asm/arch/tegra-regs.h>

#ifndef __ASSEMBLY__
# define __REG(x)	(*((volatile u32 *)IO_ADDRESS(x)))
# define __REG16(x)    (*((volatile u16 *)IO_ADDRESS(x)))

// gcc version 4.1.1 error
#if 0
# define __REG2(x,y)	\
	( __builtin_constant_p(y) ? (__REG((x) + (y))) \
			  : (*(volatile u32 *)((u32)&__REG(x) + (y))) )
#endif
# define __REG2(x,y)	(*(volatile u32 *)((u32)&__REG(x) + (y)))
#endif

#define TEGRA_IRAM_PHYS    0x40000000
#define TEGRA_IRAM_SIZE    SZ_256K
#define TEGRA_IRAM_VIRT    0xFE400000

#define TEGRA_CPU_PHYS     0x50040000
#define TEGRA_CPU_SIZE     SZ_16K
#define TEGRA_CPU_VIRT     0xFE000000

#define TEGRA_PPSB_PHYS    0x60000000
#define TEGRA_PPSB_SIZE    SZ_1M
#define TEGRA_PPSB_VIRT    0xFE200000

#define TEGRA_APB_PHYS     0x70000000
#define TEGRA_APB_SIZE     SZ_1M
#define TEGRA_APB_VIRT     0xFE300000

#define IO_TO_VIRT_BETWEEN(p, st, sz)	((p) >= (st) && (p) < ((st) + (sz)))
#define IO_TO_VIRT_XLATE(p, pst, vst)	(((p) - (pst) + (vst)))

#define IO_TO_VIRT(n) ( \
	IO_TO_VIRT_BETWEEN((n), IO_PPSB_PHYS, IO_PPSB_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IO_PPSB_PHYS, IO_PPSB_VIRT) :	\
	IO_TO_VIRT_BETWEEN((n), IO_APB_PHYS, IO_APB_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IO_APB_PHYS, IO_APB_VIRT) :	\
	IO_TO_VIRT_BETWEEN((n), IO_CPU_PHYS, IO_CPU_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IO_CPU_PHYS, IO_CPU_VIRT) :	\
	IO_TO_VIRT_BETWEEN((n), IO_IRAM_PHYS, IO_IRAM_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IO_IRAM_PHYS, IO_IRAM_VIRT) :	\
	0)

#define MAXIRQNUM                       62
#define MAXFIQNUM                       62
#define MAXSWINUM                       62

/*
 * Use SDRAM for memory
 */
#define MEM_SIZE		0x01000000

#endif
