#ifndef __ASM_PAGE_H__
#define __ASM_PAGE_H__

#include <asm/config.h>
#include <asm/pgtable.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT		12
#define PAGE_SIZE		(1 << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	(((addr) + PAGE_SIZE - 1) & PAGE_MASK)

#define round_pgup(_p)      (((_p)+(PAGE_SIZE-1))&PAGE_MASK)
#define round_pgdown(_p)    ((_p)&PAGE_MASK)

#ifdef __ASSEMBLY__
#define MAP_ENTRY(va, pa, size, attr)           \
	.word   size                            ;\
	.word   4 * ((va) >> 20)                ;\
	.word   (pa) | (attr)                   ;
#endif

#define PFN_DOWN(x)	((x)>> PAGE_SHIFT)
#define PFN_UP(x)	(((x)+PAGE_SIZE-1) >> PAGE_SHIFT)

#ifndef __ASSEMBLY__

extern pde_t *idle_pgd;

#define clear_page(_p)			memset((void *)(_p), 0, PAGE_SIZE)
#define copy_page(_t, _f)		memcpy((void *)(_t), (void *)(_f), PAGE_SIZE);

static inline int get_order_from_bytes(unsigned long size)
{
	int order;

	size = (size - 1) >> PAGE_SHIFT;
	for ( order = 0; size; order++ )
		size >>= 1;

	return order;
}

static inline int get_order_from_pages(unsigned long nr_pages)
{
	int order;

	nr_pages--;
	for ( order = 0; nr_pages; order++ )
		nr_pages >>= 1;

	return order;
}

/* Convert between Xen-heap virtual addresses and machine addresses. */
#define PAGE_OFFSET         ((unsigned long)__PAGE_OFFSET)
#define va_to_ma(va)		((unsigned long)(va)-PAGE_OFFSET)
#define ma_to_va(ma)		((void *)((unsigned long)(ma)+PAGE_OFFSET))

#define pfn_to_page(_pfn)   (frame_table + ((_pfn) - min_page))
#define phys_to_page(kaddr) (frame_table + (((kaddr) >> PAGE_SHIFT) - min_page) )
#define virt_to_page(kaddr) (frame_table + (((va_to_ma((kaddr)) >> PAGE_SHIFT)) - min_page) )
#define pfn_valid(_pfn)     (((_pfn) >= min_page) && ((_pfn) <= max_page))

#define pfn_to_phys(pfn)    ((unsigned long)(pfn) << PAGE_SHIFT)
#define phys_to_pfn(pa)     ((unsigned long)((pa) >> PAGE_SHIFT))


#define mfn_to_page(_mfn)	pfn_to_page(_mfn)
#define mfn_valid(_mfn)		pfn_valid(_mfn)

#define pte_val(x)          ((x).pte)
#define pde_val(x)          ((x).pgd)

#define GET_HVT_PAGE(pgd)           \
({                              \
	unsigned long pa;           \
	unsigned long page;			\
	pa = pde_val(idle_pgd[PGD_IDX(VECTORS_BASE)]) & PAGE_MASK;						\
	page = *(unsigned long *)(pa + (PGT_IDX(VECTORS_BASE) * sizeof(unsigned long))) & PAGE_MASK;       \
	page;\
})

#endif /* !__ASSEMBLY__ */
#endif
