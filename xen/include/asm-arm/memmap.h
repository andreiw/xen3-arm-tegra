/***************************************************************************
 * include/xen-arm/memmap.h 
 *
 * system memory map (replacement of e820.h)
 *
 * sungkwan heo <sk.heo@samsung.com>
 *
 */

#ifndef __XEN_ASM_MEMMAP_HEADER
#define __XEN_ASM_MEMMAP_HEADER

#include <asm/page.h>

#define MEMMAP_ENTRY_MAX	16

#define MEMMAP_DRAM		1
#define MEMMAP_NOR		4
#define MEMMAP_IO		16
#define MEMEAP_XENSTORE		18

#define MT_CACHECLEAN		0
#define MT_VECTOR		1
#define MT_RAM			2
#define MT_ROM			3
#define MT_IO			4

#ifndef  __ASSEMBLY__

struct map_desc {
	unsigned long virtual;
	unsigned long physical;
	unsigned long length;
	unsigned long type;
};

struct memmap_entry {
  unsigned int	addr;
  unsigned int	size;
  unsigned int	type;
};

struct system_memmap {
  int	nr_map;
  struct memmap_entry map[MEMMAP_ENTRY_MAX];
};

extern unsigned long 	init_memmap(void);
extern void		print_memmap(void);

extern struct system_memmap memmap;

#endif /* !__ASSEMBLY__ */

#ifdef __ASSEMBLY__
#define MAP_ENTRY(va, pa, size, attr)           \
	.word   size                            ;\
	.word   4 * ((va) >> 20)                ;\
	.word   (pa) | (attr)                   ;
#endif

#define PFN_DOWN(x)	( (x)>> PAGE_SHIFT )
#define PFN_UP(x)	( ((x)+PAGE_SIZE-1) >> PAGE_SHIFT )


/*
 *  Physical memory map 
 */

#define MEMMAP_XEN_START_PADDR				MEMMAP_DRAM_ADDR
#define MEMMAP_XEN_SIZE					0x00200000		// 2MB
#define MEMMAP_XEN_END_PADDR				(MEMMAP_XEN_START_PADDR + MEMMAP_XEN_SIZE)

#define MEMMAP_GUEST_0_START_PADDR			MEMMAP_XEN_END_PADDR
#define MEMMAP_GUEST_0_SIZE				(CONFIG_MEMMAP_GUEST_0_SIZE - MEMMAP_XEN_SIZE) // Dom0 size = size - xen size
#define MEMMAP_GUEST_0_END_PADDR			(MEMMAP_GUEST_0_START_PADDR + MEMMAP_GUEST_0_SIZE)

#define MEMMAP_GUEST_1_START_PADDR			MEMMAP_GUEST_0_END_PADDR
#define MEMMAP_GUEST_1_SIZE				CONFIG_MEMMAP_GUEST_1_SIZE
#define MEMMAP_GUEST_1_END_PADDR			(MEMMAP_GUEST_1_START_PADDR + MEMMAP_GUEST_1_SIZE)

#define MEMMAP_GUEST_2_START_PADDR                      MEMMAP_GUEST_1_END_PADDR
#define MEMMAP_GUEST_2_SIZE                             CONFIG_MEMMAP_GUEST_2_SIZE
#define MEMMAP_GUEST_2_END_PADDR                        (MEMMAP_GUEST_2_START_PADDR + MEMMAP_GUEST_2_SIZE)

#define MEMMAP_GUEST_3_START_PADDR                      MEMMAP_GUEST_2_END_PADDR
#define MEMMAP_GUEST_3_SIZE                             CONFIG_MEMMAP_GUEST_3_SIZE
#define MEMMAP_GUEST_3_END_PADDR                        (MEMMAP_GUEST_3_START_PADDR + MEMMAP_GUEST_3_SIZE)

// when using ramfs
#define MEMMAP_GUEST_0_ELF_MAX_SIZE		        CONFIG_MEMMAP_GUEST_0_ELF_MAX_SIZE
#define MEMMAP_GUEST_1_ELF_MAX_SIZE		        CONFIG_MEMMAP_GUEST_1_ELF_MAX_SIZE
#define MEMMAP_GUEST_2_ELF_MAX_SIZE		        CONFIG_MEMMAP_GUEST_2_ELF_MAX_SIZE
#define MEMMAP_GUEST_3_ELF_MAX_SIZE		        CONFIG_MEMMAP_GUEST_3_ELF_MAX_SIZE

// when using ramdisk
#define MEMMAP_GUEST_0_RAMDISK_SIZE	                CONFIG_MEMMAP_GUEST_0_RAMDISK_SIZE	
#define MEMMAP_GUEST_1_RAMDISK_SIZE		        CONFIG_MEMMAP_GUEST_1_RAMDISK_SIZE
#define MEMMAP_GUEST_2_RAMDISK_SIZE		        CONFIG_MEMMAP_GUEST_2_RAMDISK_SIZE
#define MEMMAP_GUEST_3_RAMDISK_SIZE		        CONFIG_MEMMAP_GUEST_3_RAMDISK_SIZE


#ifndef XEN_GUEST_IMAGES_IN_FLASH			
	#define MEMMAP_XEN_BIN_IMAGE_PADDR			(MEMMAP_DRAM_ADDR + 0x00008000)
	#define MEMMAP_GUEST_0_ELF_IMAGE_PADDR		(MEMMAP_GUEST_0_END_PADDR       - MEMMAP_GUEST_0_ELF_MAX_SIZE)
	#define MEMMAP_GUEST_0_RAMDISK_IMAGE_PADDR	(MEMMAP_GUEST_0_ELF_IMAGE_PADDR - MEMMAP_GUEST_0_RAMDISK_SIZE)
	#define MEMMAP_GUEST_1_ELF_IMAGE_PADDR		(MEMMAP_GUEST_1_END_PADDR       - MEMMAP_GUEST_1_ELF_MAX_SIZE)
	#define MEMMAP_GUEST_1_RAMDISK_IMAGE_PADDR	(MEMMAP_GUEST_1_ELF_IMAGE_PADDR - MEMMAP_GUEST_1_RAMDISK_SIZE)
        #define MEMMAP_GUEST_2_ELF_IMAGE_PADDR          (MEMMAP_GUEST_2_END_PADDR       - MEMMAP_GUEST_2_ELF_MAX_SIZE)
        #define MEMMAP_GUEST_2_RAMDISK_IMAGE_PADDR      (MEMMAP_GUEST_2_ELF_IMAGE_PADDR - MEMMAP_GUEST_2_RAMDISK_SIZE)
        #define MEMMAP_GUEST_3_ELF_IMAGE_PADDR          (MEMMAP_GUEST_3_END_PADDR       - MEMMAP_GUEST_3_ELF_MAX_SIZE)
        #define MEMMAP_GUEST_3_RAMDISK_IMAGE_PADDR      (MEMMAP_GUEST_3_ELF_IMAGE_PADDR - MEMMAP_GUEST_3_RAMDISK_SIZE)

#else
	#define MEMMAP_NOR_FLASH_BASE_PADDR			MEMMAP_NOR_ADDR
	/* NOR  -  blob: 16KB,   param: 16KB,   xen: 432KB,   dom0: 1536KB,   root fs: 30MB */
	/* NAND -  dom1: 2048KB, root fs: 30MB, dom2: 2048KB, root fs: 30MB */
	#define MEMMAP_BLOB_PADDR					(MEMMAP_NOR_FLASH_BASE_PADDR + 0x00000000)	
	#define MEMMAP_BLOB_PARAM_PADDR				(MEMMAP_NOR_FLASH_BASE_PADDR + 0x00010000)
	#define MEMMAP_XEN_BIN_IMAGE_PADDR			(MEMMAP_NOR_FLASH_BASE_PADDR + 0x00014000)
	#define MEMMAP_GUEST_0_ELF_IMAGE_PADDR		(MEMMAP_NOR_FLASH_BASE_PADDR + 0x00080000)
	#define MEMMAP_GUEST_0_RAMDISK_IMAGE_PADDR	(MEMMAP_NOR_FLASH_BASE_PADDR + 0x00200000)
	#define MEMMAP_GUEST_1_ELF_IMAGE_PADDR		// undefined 
	#define MEMMAP_GUEST_1_RAMDISK_IMAGE_PADDR	// undefined
#endif /* XEN_GUEST_IMAGES_IN_FLASH */


#endif /* !__XEN_ASM_MEMMAP_HEADER */


