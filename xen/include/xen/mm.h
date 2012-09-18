/******************************************************************************
 * include/xen/mm.h
 * 
 * Definitions for memory pages, frame numbers, addresses, allocations, etc.
 * 
 * Note that Xen must handle several different physical 'address spaces' and
 * there is a consistent terminology for these:
 * 
 * 1. gpfn/gpaddr: A guest-specific pseudo-physical frame number or address.
 * 2. gmfn/gmaddr: A machine address from the p.o.v. of a particular guest.
 * 3. mfn/maddr:   A real machine frame number or address.
 * 4. pfn/paddr:   Used in 'polymorphic' functions that work across all
 *                 address spaces, depending on context. See the pagetable
 *                 conversion macros in asm-x86/page.h for examples.
 *                 Also 'paddr_t' is big enough to store any physical address.
 * 
 * This scheme provides consistent function and variable names even when
 * different guests are running in different memory-management modes.
 * 1. A guest running in auto-translated mode (e.g., shadow_mode_translate())
 *    will have gpfn == gmfn and gmfn != mfn.
 * 2. A paravirtualised x86 guest will have gpfn != gmfn and gmfn == mfn.
 * 3. A paravirtualised guest with no pseudophysical overlay will have
 *    gpfn == gpmfn == mfn.
 * 
 * Copyright (c) 2002-2006, K A Fraser <keir@xensource.com>
 */

#ifndef __XEN_MM_H__
#define __XEN_MM_H__

#include <xen/config.h>
#include <xen/types.h>
#include <xen/list.h>
#include <xen/spinlock.h>

struct domain;
struct page_info;

/* Boot-time allocator. Turns into generic allocator after bootstrap. */
paddr_t boot_allocator_init(paddr_t bitmap_start);
void boot_pages_init(paddr_t ps, paddr_t pe);
unsigned long boot_pages_alloc(unsigned long nr_pfns, unsigned long pfn_align);
void boot_allocator_end(void);

/* Generic allocator. These functions are *not* interrupt-safe. */
void pages_init(unsigned int zone, struct page_info *pg, unsigned long nr_pages);
struct page_info *pages_alloc(unsigned int zone, unsigned int order);
void pages_free(unsigned int zone, struct page_info *pg, unsigned int order);
void pages_scrub(void);

/* Xen suballocator. These functions are interrupt-safe. */
void pages_m_init(paddr_t ps, paddr_t pe);
void *pages_m_alloc(unsigned int order);
void pages_m_free(void *v, unsigned int order);
#define pages_m_alloc1() (pages_m_alloc(0))
#define pages_m_free1(v) (pages_m_free(v,0))

/* Domain suballocator. These functions are *not* interrupt-safe.*/
void pages_u_init(paddr_t ps, paddr_t pe);
struct page_info *pages_u_alloc(struct domain *d, unsigned int order, unsigned int flags);
void pages_u_free(struct page_info *pg, unsigned int order);
unsigned long pages_u_avail(void);
#define pages_u_alloc1(d) (pages_u_alloc(d,0,0))
#define pages_u_free1(p)  (pages_u_free(p,0))

#define ALLOC_DOM_DMA 1

/* Up to 2^20 pages can be allocated at once. */
#define MAX_ORDER 20

/* Automatic page scrubbing for dead domains. */
extern struct list_head page_scrub_list;
#define page_scrub_schedule_work()              \
    do {                                        \
        if ( !list_empty(&page_scrub_list) ) {   \
			printf("page scrub\n");				\
            raise_softirq(PAGE_SCRUB_SOFTIRQ);  \
		}										\
    } while ( 0 )

#include <asm/mm.h>

#ifndef sync_pagetable_state
#define sync_pagetable_state(d) ((void)0)
#endif

int guest_remove_page(struct domain *d, unsigned long gmfn);

#endif /* __XEN_MM_H__ */
