#ifndef __TEGRA_CONFIG_H__
#define __TEGRA_CONFIG_H__

#define HZ			100
#define PHYS_OFFSET   		(0x00000000UL)

#define KERNEL_PHYS_BASE        (0x100000)

/* This needs to match asm/config.h, xen.lds.S */
#define KERNEL_VIRT_BASE        (0xff000000)

#define __PAGE_OFFSET           (KERNEL_VIRT_BASE - KERNEL_PHYS_BASE)

#define MEMMAP_DRAM_ADDR	PHYS_OFFSET
#define MEMMAP_DRAM_SIZE	0x04000000

/* Will go away. */
#define MEMMAP_NOR_ADDR		0xC8000000
#define MEMMAP_NOR_SIZE		0x02000000

#endif
