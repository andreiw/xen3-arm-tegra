/*
 * page_alloc.c
 *
 * Simple buddy heap allocator for Xen.
 *
 * Copyright (C) 2002-2004 K A Fraser
 * Copyright (C) 2012 Andrei Warkentin <andrey.warkentin@gmail.com>
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

#include <xen/kernel.h>
#include <xen/config.h>
#include <xen/spinlock.h>
#include <asm/io.h>
#include <xen/list.h>
#include <xen/compiler.h>
#include <xen/cpumask.h>
#include <xen/mm.h>
#include <asm/system.h>
#include <asm/page.h>
#include <xen/kernel.h>

#include <xen/perfc.h>
#include <xen/sched.h>
#include <xen/shadow.h>
#include <xen/irq_cpustat.h>
#include <xen/softirq.h>
#include <xen/domain_page.h>

/*
 * Comma-separated list of hexadecimal page numbers containing bad bytes.
 * e.g. 'badpage=0x3f45,0x8a321'.
 */
static char opt_badpage[100] = "";

static spinlock_t page_scrub_lock = SPIN_LOCK_UNLOCKED;
LIST_HEAD(page_scrub_list);

/*
 * ALLOCATION BITMAP
 *  One bit per page of memory. Bit set => page is allocated.
 */

static unsigned long *alloc_bitmap;
#define PAGES_PER_MAPWORD (sizeof(unsigned long) * 8)

#define allocated_in_map(_pn)                               \
   ( !! (alloc_bitmap[((_pn)-min_page)/PAGES_PER_MAPWORD] & \
         (1UL<<(((_pn)-min_page)&(PAGES_PER_MAPWORD-1)))) )

#define allocated_status_in_map(_pn)                     \
   ((alloc_bitmap[((_pn)-min_page)/PAGES_PER_MAPWORD] &  \
     (1UL<<(((_pn)-min_page)&(PAGES_PER_MAPWORD-1)))) )

/*
 * \brief updates alloc_bitmap
 * Hint regarding bitwise arithmetic in map_{alloc,free}:
 *  -(1<<n)  sets all bits >= n.
 *  (1<<n)-1 sets all bits <  n.
 * Variable names in map_{alloc,free}:
 *  *_idx == Index into `alloc_bitmap' array.
 *  *_off == Bit offset within an element of the `alloc_bitmap' array.
 * \param first_page: the first of the pages to allocate
 * \param nr_pages: the number of pages to allocate
 */
static void map_alloc(unsigned long first_page, unsigned long nr_pages)
{
   unsigned long start_off, end_off, curr_idx, end_idx;

#ifndef NDEBUG
   unsigned long i;
   /* Check that the block isn't already allocated. */
   for (i = 0; i < nr_pages; i++) {
      if(allocated_in_map(first_page + i))
         ASSERT(!allocated_in_map(first_page + i));
   }
#endif

   curr_idx  = (first_page-min_page) / PAGES_PER_MAPWORD;
   start_off = (first_page-min_page) & (PAGES_PER_MAPWORD-1);
   end_idx   = (first_page-min_page + nr_pages) / PAGES_PER_MAPWORD;
   end_off   = (first_page-min_page + nr_pages) & (PAGES_PER_MAPWORD-1);

   if (curr_idx == end_idx) {
      alloc_bitmap[curr_idx] |= ((1UL<<end_off)-1) & -(1UL<<start_off);
   } else {
      alloc_bitmap[curr_idx] |= -(1UL<<start_off);
      while (++curr_idx < end_idx) alloc_bitmap[curr_idx] = ~0UL;
      alloc_bitmap[curr_idx] |= (1UL<<end_off)-1;
   }
}


static void map_free(unsigned long first_page, unsigned long nr_pages)
{
   unsigned long start_off, end_off, curr_idx, end_idx;

#ifndef NDEBUG
   unsigned long i;

   /* Check that the block isn't already freed. */
   for (i = 0; i < nr_pages; i++) {
      ASSERT(allocated_in_map(first_page + i));
   }
#endif

   curr_idx  = (first_page-min_page) / PAGES_PER_MAPWORD;
   start_off = (first_page-min_page) & (PAGES_PER_MAPWORD-1);
   end_idx   = (first_page-min_page + nr_pages) / PAGES_PER_MAPWORD;
   end_off   = (first_page-min_page + nr_pages) & (PAGES_PER_MAPWORD-1);

   if (curr_idx == end_idx) {
      alloc_bitmap[curr_idx] &= -(1UL<<end_off) | ((1UL<<start_off)-1);
   } else {
      alloc_bitmap[curr_idx] &= (1UL<<start_off)-1;
      while (++curr_idx != end_idx) alloc_bitmap[curr_idx] = 0;
      alloc_bitmap[curr_idx] &= -(1UL<<end_off);
   }
}


/*
 * BOOT-TIME ALLOCATOR
 */

/* Initialise allocator to handle up to @max_page pages. */
physaddr_t boot_allocator_init(physaddr_t bitmap_start)
{
   unsigned long bitmap_size;

   bitmap_start = ROUND_UP(bitmap_start, PAGE_SIZE);

   /*
    * Allocate space for the allocation bitmap. Include an extra longword
    * of padding for possible overrun in map_alloc and map_free.
    */
   bitmap_size  = (max_page - min_page) / 8;
   bitmap_size += sizeof(unsigned long);
   bitmap_size  = ROUND_UP(bitmap_size, PAGE_SIZE);
   alloc_bitmap = (unsigned long *)phys_to_virt(bitmap_start);

   /* All allocated by default. */
   memset(alloc_bitmap, ~0, bitmap_size);

   return bitmap_start + bitmap_size;
}


void boot_pages_init(physaddr_t ps, physaddr_t pe)
{
   unsigned long bad_pfn;
   char *p;

   ps = ROUND_UP(ps, PAGE_SIZE);
   pe = ROUND_DOWN(pe, PAGE_SIZE);
   if (pe <= ps) {
      return;
   }

   map_free(ps >> PAGE_SHIFT, (pe - ps) >> PAGE_SHIFT);

   /* Check new pages against the bad-page list. */
   p = opt_badpage;
   while (*p != '\0') {
      bad_pfn = simple_strtoul(p, &p, 0);

      if (*p == ',') {
         p++;
      } else if (*p != '\0') {
         break;
      }

      if ((bad_pfn < max_page) && !allocated_in_map(bad_pfn)) {
         printk("Marking page %lx as bad\n", bad_pfn);
         map_alloc(bad_pfn, 1);
      }
   }
}


void boot_pages_reserve(paddr_t start, paddr_t end)
{
   start = ROUND_UP(start, PAGE_SIZE);
   end = ROUND_DOWN(end, PAGE_SIZE);
   if (end <= start) {
      return;
   }

   map_alloc(start >> PAGE_SHIFT, (end - start) >> PAGE_SHIFT);
}


/*
 * BINARY BUDDY ALLOCATOR
 */

#define MEMZONE_MAPPED       0
#define MEMZONE_UNMAPPED     1
#define MEMZONE_UNMAPPED_DMA 2
#define NR_ZONES             (MEMZONE_UNMAPPED_DMA + 1)

#define pfn_dom_zone_type(_pfn)                                \
   (((_pfn) <= MAX_UNMAPPED_DMA_PFN) ? MEMZONE_UNMAPPED_DMA : MEMZONE_UNMAPPED)

/* Up to 2^20 pages can be allocated at once. */
#define MAX_ORDER 20
static struct list_head heap[NR_ZONES][MAX_ORDER+1];

static unsigned long avail[NR_ZONES];

static spinlock_t heap_lock = SPIN_LOCK_UNLOCKED;

/*
 * Verify that the heap is not seemingly corrupted.
 *
 * Does not catch all kinds of corruption.
 */
static void pages_check()
{
   int zone;
   int order;
   int ocount;
   struct list_head *list;
   struct page_info *page;

   for (zone = 0; zone < NR_ZONES; zone++) {
      printk("Checking zone %u\n", zone);
      order = MAX_ORDER;
      while (order) {
         ocount = 0;
         list = heap[zone][order].next;
         while (list != &heap[zone][order]) {
            ocount++;
            page = list_entry(list, struct page_info, list);
            if (PFN_ORDER(page) != order) {
               printk("Uh, oh heap[%u][%u] page 0x%x order %u (phys 0x%x) )doesn't match\n",
                      zone, order, page, PFN_ORDER(page), page_to_phys(page));
               panic("corrupted heap\n");
            }
            list = list->next;
         }
         if (ocount) {
            printk("heap[%u][%u] = %u page_infos\n", zone, order, ocount);
         }
         order--;
      }
   }
}

/**
 * \brief end of boot allocator setup
 * \brief free heap pages are linked to MEMZONE_UNMAPPED_DMA zone lists or MEMZONE_UNMAPPED zone lists.
 */
void boot_allocator_end(void)
{
   unsigned long i, j;
   int curr_free = 0, next_free = 0;

   memset(avail, 0, sizeof(avail));

   for (i = 0; i < NR_ZONES; i++) {
      for (j = 0; j <= MAX_ORDER; j++) {
         INIT_LIST_HEAD(&heap[i][j]);
      }
   }

   /*
    * Pages that are free now go to the domain sub-allocator.
    */
   next_free = !allocated_in_map(min_page);
   for (i = min_page; i < max_page; i++) {
      curr_free = next_free;
      next_free = !allocated_in_map(i+1);
      if (next_free) {
         map_alloc(i+1, 1); /* prevent merging in pages_free() */
      }

      /* "pfn_dom_zone_type" checkes whether this page belongs to MEMZONE_UNMAPPED_DMA zone or MEMZONE_UNMAPPED zone. */
      if (curr_free) {
         pages_free(pfn_dom_zone_type(i), mfn_to_page(i), 0);
      }
   }

   /* pages_check(); */
}


/* Hand the specified arbitrary page range to the specified heap zone. */
void pages_init(unsigned int zone,
                struct page_info *pg,
                unsigned long nr_pages)
{
   unsigned long i;

   ASSERT(zone < NR_ZONES);

   for (i = 0; i < nr_pages; i++) {
      pages_free(zone, pg + i, 0);
   }
}


static inline void merge_pages(int zone,
                               struct page_info *pg,
                               int order)
{
   int i;
   unsigned long mask;

   while(order < MAX_ORDER) {
      mask = 1 << order;
      if ((page_to_mfn(pg) & mask)) {
         if (allocated_in_map(page_to_pfn(pg)-mask) ||
             (PFN_ORDER(pg-mask) != order)) {
            break;
         }
         list_del_init(&(pg-mask)->list);
         pg -= mask;
      } else {
         if (allocated_in_map(page_to_pfn(pg) + mask) ||
             (PFN_ORDER(pg+mask) != order)) {
            break;
         }
         list_del_init(&(pg+mask)->list);
      }

      order++;
   }

   list_add_tail(&pg->list, &heap[zone][order]);

   for (i = 0; i < (1 << order); i++) {
      PFN_ORDER(pg + i) = order;
   }
}


/* Allocate 2^@order contiguous pages. */
struct page_info *pages_alloc(unsigned int zone,
                              unsigned int order)
{
   int i;
   struct page_info *pg;

   ASSERT(zone < NR_ZONES);

   if (unlikely(order > MAX_ORDER)) {
      return NULL;
   }

   spin_lock(&heap_lock);

   /* Find smallest order which can satisfy the request. */
   for (i = order; i <= MAX_ORDER; i++) {
      if (!list_empty(&heap[zone][i])) {
         goto found;
      }
   }

   /* No suitable memory blocks. Fail the request. */
   spin_unlock(&heap_lock);

   return NULL;
found:
   pg = list_entry(heap[zone][i].next, struct page_info, list);
   list_del_init(&pg->list);

   /* We may have to halve the chunk a number of times. */
   while (i != order) {
      PFN_ORDER(pg) = --i;
      list_add_tail(&pg->list, &heap[zone][i]);
      pg += 1 << i;
   }

   map_alloc(page_to_pfn(pg), 1 << order);
   avail[zone] -= 1 << order;

   spin_unlock(&heap_lock);
   return pg;
}


/* Free 2^@order set of pages. */
void pages_free(unsigned int zone,
                     struct page_info *pg,
                     unsigned int order)
{
   unsigned long i, mask;

   ASSERT(zone < NR_ZONES);
   ASSERT(order <= MAX_ORDER);

   spin_lock(&heap_lock);

   map_free(page_to_pfn(pg), 1 << order);
   avail[zone] += 1 << order;

   if (!list_empty(&pg->list)) {
      list_del_init(&pg->list);
   }


   /* Merge chunks as far as possible. */
   while (order < MAX_ORDER) {
      mask = 1 << order;

      if ((page_to_pfn(pg) & mask)) {

         /* Merge with predecessor block? */
         if (allocated_in_map(page_to_pfn(pg)-mask) ||
             (PFN_ORDER(pg-mask) != order)) {
            break;
         }

         list_del_init(&(pg-mask)->list);
         pg -= mask;
      }
      else {

         /* Merge with successor block? */
         if (allocated_in_map(page_to_pfn(pg)+mask) ||
             (PFN_ORDER(pg+mask) != order)) {
            break;
         }

         list_del_init(&(pg+mask)->list);
      }

      order++;
   }

   list_add_tail(&pg->list, &heap[zone][order]);

   for (i = 0; i < (1 << order); i++) {
      PFN_ORDER(pg + i) = order;
   }

   spin_unlock(&heap_lock);
}


/*
 * Scrub all unallocated pages in all heap zones. This function is more
 * convoluted than appears necessary because we do not want to continuously
 * hold the lock or disable interrupts while scrubbing very large memory areas.
 */

void pages_scrub(void)
{
   void *p;
   unsigned long pfn;
   int cpu = smp_processor_id();

   printk("Scrubbing Free RAM: ");

   for (pfn = min_page; pfn < max_page; pfn++) {

      /* Every 100MB, print a progress dot. */
      if ((pfn % ((100*1024*1024)/PAGE_SIZE)) == 0) {
         printk(".");
      }

      if (unlikely(softirq_pending(cpu))) {
         do_softirq();
      }

      /* Quick lock-free check. */
      if (allocated_in_map(pfn)) {
         continue;
      }

      spin_lock_irq(&heap_lock);

      /* Re-check page status with lock held. */
      if (!allocated_in_map(pfn)) {
         if (IS_M_FRAME(mfn_to_page(pfn))) {
            p = page_to_virt(mfn_to_page(pfn));
            memguard_unguard_range(p, PAGE_SIZE);
            clear_page(p);
            memguard_guard_range(p, PAGE_SIZE);
         } else {
            p = map_domain_page(pfn);
            clear_page(p);
            unmap_domain_page(p);
         }
      }

      spin_unlock_irq(&heap_lock);
   }

   printk("done.\n");
}


/*
 * Suballocator for linearly mapped memory used only by Xen kernel.
 */

void pages_m_init(physaddr_t ps, physaddr_t pe)
{
   unsigned long flags;

   ps = ROUND_UP(ps, PAGE_SIZE);
   pe = ROUND_DOWN(pe, PAGE_SIZE);
   if (pe <= ps) {
      return;
   }

   memguard_guard_range(phys_to_virt(ps), pe - ps);

   /*
    * Yuk! Ensure there is a one-page buffer between mapped and unmapped
    * zones, to prevent merging of power-of-two blocks across the zone
    * boundary.
    *
    * AndreiW: Needs to be rewritten. *sigh*.
    */

   if (!IS_M_FRAME(phys_to_page(pe))) {
      pe -= PAGE_SIZE;
   }

   local_irq_save(flags);

   /* This is the guard page. */
   map_alloc(pe >> PAGE_SHIFT, 1);

   /*
    * AndreiW:
    * map_free will complain these pages aren't allocated,
    * so do it. How did this shit ever run anywhere?
    */

   map_alloc(ps >> PAGE_SHIFT, (pe - ps) >> PAGE_SHIFT);
   /* pages_check(); */
   pages_init(MEMZONE_MAPPED, phys_to_page(ps), (pe - ps) >> PAGE_SHIFT);
   /* pages_check(); */
   local_irq_restore(flags);
}


void *pages_m_alloc(unsigned int order)
{
   unsigned long flags;
   struct page_info *pg;
   int i;

   local_irq_save(flags);
   pg = pages_alloc(MEMZONE_MAPPED, order);
   local_irq_restore(flags);

   if (unlikely(pg == NULL)) {
      goto no_memory;
   }

   memguard_unguard_range(page_to_virt(pg), 1 << (order + PAGE_SHIFT));

   for (i = 0; i < (1 << order); i++)
   {
      pg[i].count_info        = 0;
      pg[i].u.inuse._domain   = 0;
      pg[i].u.inuse.type_info = 0;
   }

   return page_to_virt(pg);
no_memory:
   printk("Cannot handle page request order %d!\n", order);
   return NULL;
}


void pages_m_free(void *v, unsigned int order)
{
   unsigned long flags;

   if (v == NULL) {
      return;
   }

   memguard_guard_range(v, 1 << (order + PAGE_SHIFT));

   local_irq_save(flags);
   pages_free(MEMZONE_MAPPED, virt_to_page(v), order);
   local_irq_restore(flags);
}


/*
 * DOMAIN-HEAP SUB-ALLOCATOR
 */

void pages_u_init(physaddr_t ps, physaddr_t pe)
{
   unsigned long s_tot, e_tot, s_dma, e_dma, s_nrm, e_nrm;

   ASSERT(!in_irq());

   s_tot = ROUND_UP(ps, PAGE_SIZE) >> PAGE_SHIFT;
   e_tot = ROUND_DOWN(pe, PAGE_SIZE) >> PAGE_SHIFT;

   s_dma = min(s_tot, MAX_UNMAPPED_DMA_PFN + 1);
   e_dma = min(e_tot, MAX_UNMAPPED_DMA_PFN + 1);
   if (s_dma < e_dma) {
      pages_init(MEMZONE_UNMAPPED_DMA, mfn_to_page(s_dma), e_dma - s_dma);
   }

   s_nrm = max(s_tot, MAX_UNMAPPED_DMA_PFN + 1);
   e_nrm = max(e_tot, MAX_UNMAPPED_DMA_PFN + 1);
   if (s_nrm < e_nrm) {
      pages_init(MEMZONE_UNMAPPED, mfn_to_page(s_nrm), e_nrm - s_nrm);
   }
}


struct page_info *pages_u_alloc(struct domain *d,
                                unsigned int order,
                                unsigned int flags)
{
   struct page_info *pg = NULL;
   /* cpumask_t mask; */
   int i;

   ASSERT(!in_irq());

   if (!(flags & ALLOC_DOM_DMA)) {
      pg = pages_alloc(MEMZONE_UNMAPPED, order);
   }

   if (pg == NULL) {
      if ((pg = pages_alloc(MEMZONE_UNMAPPED_DMA, order)) == NULL) {
         return NULL;

      }
   }

   /* mask = pg->u.free.cpumask; */
   /* tlbflush_filter(mask, pg->tlbflush_timestamp); */

   pg->count_info        = 0;
   pg->u.inuse._domain   = 0;
   pg->u.inuse.type_info = 0;

   for (i = 1; i < (1 << order); i++) {

      /* Add in any extra CPUs that need flushing because of this page. */
      /* cpumask_t extra_cpus_mask; */
      /* cpus_andnot(extra_cpus_mask, pg[i].u.free.cpumask, mask); */
      /* tlbflush_filter(extra_cpus_mask, pg[i].tlbflush_timestamp); */
      /* cpus_or(mask, mask, extra_cpus_mask); */

      pg[i].count_info        = 0;
      pg[i].u.inuse._domain   = 0;
      pg[i].u.inuse.type_info = 0;
   }

   /* if (unlikely(!cpus_empty(mask))) */
   /* { */
   /*    perfc_incrc(need_flush_tlb_flush); */
   /*    flush_tlb_mask(mask); */
   /* } */

   if (d == NULL) {
      return pg;
   }

   spin_lock(&d->page_alloc_lock);

   if(unlikely(test_bit(_DOMF_dying, &d->domain_flags))) {
      if (unlikely(test_bit(_DOMF_dying, &d->domain_flags)) ||
           unlikely((d->tot_pages + (1 << order)) > d->max_pages)) {
         DPRINTK(3, "Over-allocation for domain %u: %u > %u\n",
                 d->domain_id, d->tot_pages + (1 << order), d->max_pages);
         DPRINTK(3, "...or the domain is dying (%d)\n",
                 !!test_bit(_DOMF_dying, &d->domain_flags));
         spin_unlock(&d->page_alloc_lock);
         pages_free(pfn_dom_zone_type(page_to_pfn(pg)), pg, order);
         printk("[PSY]break point [3-1]!\n");
         return NULL;
      }
   }

   if (unlikely(d->tot_pages == 0)) {
      get_knownalive_domain(d);
   }

   d->tot_pages += 1 << order;

   for (i = 0; i < (1 << order); i++) {
      page_set_owner(&pg[i], d);
      wmb(); /* Domain pointer must be visible before updating refcnt. */
      pg[i].count_info |= PGC_allocated | 1;
      list_add_tail(&pg[i].list, &d->page_list);
   }

   spin_unlock(&d->page_alloc_lock);

   return pg;
}


void pages_u_free(struct page_info *pg, unsigned int order)
{
   int            i, drop_dom_ref;
   struct domain *d = page_get_owner(pg);

   ASSERT(!in_irq());

   if (unlikely(IS_M_FRAME(pg))) {

      /* NB. May recursively lock from relinquish_memory(). */
      spin_lock_recursive(&d->page_alloc_lock);

      for (i = 0; i < (1 << order); i++) {
         list_del_init(&pg[i].list);
      }

      d->xenheap_pages -= 1 << order;
      drop_dom_ref = (d->xenheap_pages == 0);

      spin_unlock_recursive(&d->page_alloc_lock);
   } else if (likely(d != NULL)) {

      /* NB. May recursively lock from relinquish_memory(). */
      spin_lock_recursive(&d->page_alloc_lock);

      for (i = 0; i < (1 << order); i++) {
         shadow_drop_references(d, &pg[i]);
         ASSERT((pg[i].u.inuse.type_info & PGT_count_mask) == 0);

         /* pg[i].tlbflush_timestamp  = tlbflush_current_time(); */
         /* pg[i].u.free.cpumask      = d->domain_dirty_cpumask; */

         list_del_init(&pg[i].list);
      }

      d->tot_pages -= 1 << order;
      drop_dom_ref = (d->tot_pages == 0);

      spin_unlock_recursive(&d->page_alloc_lock);

      if (likely(!test_bit(_DOMF_dying, &d->domain_flags))) {
         pages_free(pfn_dom_zone_type(page_to_pfn(pg)), pg, order);
      } else {

         /*
          * Normally we expect a domain to clear pages before freeing them,
          * if it cares about the secrecy of their contents. However, after
          * a domain has died we assume responsibility for erasure.
          */

         /* for ( i = 0; i < (1 << order); i++ ) { */
         /*    spin_lock(&page_scrub_lock); */
         /*    list_add(&pg[i].list, &page_scrub_list); */
         /*    spin_unlock(&page_scrub_lock); */
         /* } */

         pages_free(pfn_dom_zone_type(page_to_pfn(pg)), pg, 0);
      }
   } else {

      /* Freeing anonymous domain-heap pages. */

      /* for (i = 0; i < (1 << order); i++) { */
      /*    pg[i].u.free.cpumask = CPU_MASK_NONE; */
      /* } */

      pages_free(pfn_dom_zone_type(page_to_pfn(pg)), pg, order);
      drop_dom_ref = 0;
   }

   if (drop_dom_ref) {
      put_domain(d);
   }
}


unsigned long pages_u_avail(void)
{
   return avail[MEMZONE_UNMAPPED] + avail[MEMZONE_UNMAPPED_DMA];
}


/*
 * PAGE SCRUBBING
 */

static void page_scrub_softirq(void)
{
   struct list_head *ent;
   struct page_info  *pg;
   void             *p;
   int               i;
   s_time_t          start = NOW();

   /* Aim to do 1ms of work (ten percent of a 10ms jiffy). */
   do {
      spin_lock(&page_scrub_lock);

      if (unlikely((ent = page_scrub_list.next) == &page_scrub_list)) {
         spin_unlock(&page_scrub_lock);
         return;
      }

      /* Peel up to 16 pages from the list. */
      for (i = 0; i < 16; i++) {
         if (ent->next == &page_scrub_list) {
            break;
         }
         ent = ent->next;
      }

      /* Remove peeled pages from the list. */
      ent->next->prev = &page_scrub_list;
      page_scrub_list.next = ent->next;

      spin_unlock(&page_scrub_lock);

      /* Working backwards, scrub each page in turn. */
      while (ent != &page_scrub_list) {
         pg = list_entry(ent, struct page_info, list);
         ent = ent->prev;
         p = map_domain_page(page_to_pfn(pg));
         clear_page(p);
         unmap_domain_page(p);
         pages_free(pfn_dom_zone_type(page_to_pfn(pg)), pg, 0);
      }
   } while ((NOW() - start) < MILLISECS(5));
}


static __init int page_scrub_init(void)
{
   open_softirq(PAGE_SCRUB_SOFTIRQ, page_scrub_softirq);
   return 0;
}

__initcall(page_scrub_init);

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
