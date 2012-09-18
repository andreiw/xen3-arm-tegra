#ifndef __TEGRA_CONFIG_H__
#define __TEGRA_CONFIG_H__

#define HZ			100

#define KERNEL_PHYS_BASE        (0x100000)
#define KERNEL_VIRT_BASE        (0xff000000)
#define __PAGE_OFFSET           (KERNEL_VIRT_BASE - KERNEL_PHYS_BASE)

/* This should become dynamic. */
#define KERNEL_HEAP_SIZE        (2 * 1024 * 1024)
#endif
