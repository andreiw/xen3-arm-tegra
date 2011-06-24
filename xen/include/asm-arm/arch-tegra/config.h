#ifndef __TEGRA_CONFIG_H__
#define __TEGRA_CONFIG_H__

#define HZ			100
#define PHYS_OFFSET   		(0x00000000UL)

/* PGD at 1MB, kernel at 1MB + sizeof(PGD). */
#define KERNEL_PHYS             (0x104000)

#define IDLE_PG_TABLE_ADDR	(0xC0004000UL)
#define __PAGE_OFFSET           (0x3F000000UL)

#define MEMMAP_DRAM_ADDR	PHYS_OFFSET
#define MEMMAP_DRAM_SIZE	0x04000000

/* Will go away. */
#define MEMMAP_NOR_ADDR		0xC8000000
#define MEMMAP_NOR_SIZE		0x02000000

#endif
