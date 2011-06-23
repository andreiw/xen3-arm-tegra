#ifndef __ASM_ARM_IO_H
#define __ASM_ARM_IO_H

#include <asm/page.h>

/*
 * Generic IO read/write.  These perform native-endian accesses.  Note
 * that some architectures will want to re-define __raw_{read,write}w.
 */
extern void __raw_writesb(void __iomem *addr, const void *data, int bytelen);
extern void __raw_writesw(void __iomem *addr, const void *data, int wordlen);
extern void __raw_writesl(void __iomem *addr, const void *data, int longlen);

extern void __raw_readsb(void __iomem *addr, void *data, int bytelen);
extern void __raw_readsw(void __iomem *addr, void *data, int wordlen);
extern void __raw_readsl(void __iomem *addr, void *data, int longlen);

/*
 * Overriding the raw address access macros.
 */
#define __raw_writeb(v,a)	(*(volatile unsigned char *)(a) = (v))
#define __raw_writew(v,a)	(*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v,a)	(*(volatile unsigned int *)(a) = (v))

#define __raw_readb(a)		(*(volatile unsigned char *)(a))
#define __raw_readw(a)		(*(volatile unsigned short *)(a))
#define __raw_readl(a)		(*(volatile unsigned int *)(a))

/*
 * Now, pick up the machine-defined IO definitions
 */
#include <asm/arch/io.h>

#define readb(c) 		({ unsigned int __v = __raw_readb(c); __v; })
#define readw(c) 		({ unsigned int __v = le16_to_cpu(__raw_readw(c)); __v; })
#define readl(c)		({ unsigned int __v = le32_to_cpu(__raw_readl(c)); __v; })
#define readb_relaxed(addr) readb(addr)
#define readw_relaxed(addr) readw(addr)
#define readl_relaxed(addr) readl(addr)

#define readsb(p,d,l)		__raw_readsb(p,d,l)
#define readsw(p,d,l)		__raw_readsw(p,d,l)
#define readsl(p,d,l)		__raw_readsl(p,d,l)

#define writeb(v,c)		__raw_writeb(v,c)
#define writew(v,c)		__raw_writew(cpu_to_le16(v),c)
#define writel(v,c)		__raw_writel(cpu_to_le32(v),c)

#define writesb(p,d,l)		__raw_writesb(p,d,l)
#define writesw(p,d,l)		__raw_writesw(p,d,l)
#define writesl(p,d,l)		__raw_writesl(p,d,l)

/**
 *  virt_to_phys    -   map virtual addresses to physical
 *  @address: address to remap
 *
 *  The returned physical address is the physical (CPU) mapping for
 *  the memory address given. It is only valid to use this function on
 *  addresses directly mapped or allocated via xmalloc.
 *
 *  This function does not give bus mappings for DMA transfers. In
 *  almost all conceivable cases a device driver should not be using
 *  this function
 */
static inline unsigned long virt_to_phys(volatile void * address)
{
    return __pa(address);
}
/**
 *  phys_to_virt    -   map physical address to virtual
 *  @address: address to remap
 *
 *  The returned virtual address is a current CPU mapping for
 *  the memory address given. It is only valid to use this function on
 *  addresses that have a kernel mapping
 *
 *  This function does not handle bus mappings for DMA transfers. In
 *  almost all conceivable cases a device driver should not be using
 *  this function
 */
static inline void * phys_to_virt(unsigned long address)
{
    return ma_to_va(address);
}

/*
 * Change "struct page_info" to physical address.
 */
#define page_to_pfn(_page)  ((unsigned long)((_page + min_page) - frame_table ))
#define page_to_phys(page)  (page_to_pfn(page) << PAGE_SHIFT)
#define page_to_virt(_page) phys_to_virt(page_to_phys(_page))


#define page_to_mfn(_page)	page_to_pfn(_page)
#endif	/* __ASM_ARM_IO_H */
