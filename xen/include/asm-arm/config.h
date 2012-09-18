#ifndef __ARM_CONFIG_H__
#define __ARM_CONFIG_H__

#include <asm/arch/config.h>

#define supervisor_mode_kernel	(0)

#define ARCH_HAS_IRQ_CONTROL	1

#ifndef STACK_ORDER
# define STACK_ORDER				0
#endif

#define STACK_SIZE				(PAGE_SIZE << STACK_ORDER)

#ifndef NDEBUG
# define MEMORY_GUARD
#endif

#define NR_CPUS					1
#define CONFIG_NR_CPUS			1

#define COMMAND_LINE_SIZE       512

#define OPT_CONSOLE_STR			"com1"

#ifndef HYPERVISOR_VIRT_START
#define HYPERVISOR_VIRT_START   0xFC000000
#endif

#define VECTORS_BASE		0xFFFF0000
#define DIRECTMAP_VIRT_END	VECTORS_BASE
#define DIRECTMAP_VIRT_START	KERNEL_VIRT_BASE
#define DIRECTMAP_PHYS_START	KERNEL_PHYS_BASE

#define MAPCACHE_VIRT_END	DIRECTMAP_VIRT_START
#define MAPCACHE_MBYTES		4
#define MAPCACHE_VIRT_START	(MAPCACHE_VIRT_END - (MAPCACHE_MBYTES << 20))

#define SHARED_INFO_LIMIT	(MAPCACHE_VIRT_START)
#define SHARED_INFO_BASE	(SHARED_INFO_LIMIT - (1 << 20))

#define IOREMAP_VIRT_END	SHARED_INFO_BASE
#define IOREMAP_VIRT_START	HYPERVISOR_VIRT_START

#define MAX_UNMAPPED_DMA_PFN	0xFFFFFUL /* 32 addressable bits */

/* Must match xen.lds.S */
#define KERNEL_LINK_OFFSET	0x8000

#ifndef __ASSEMBLY__
extern unsigned long _start; /* standard ELF symbol */
extern unsigned long _end;   /* standard ELF symbol */
extern unsigned long xenheap_phys_end; /* user-configurable */

#endif

#define ELFSIZE	32

#endif



