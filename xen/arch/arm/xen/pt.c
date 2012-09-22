/*
 * pt.c - page table manipulation code.
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <xen/kernel.h>
#include <asm/config.h>
#include <asm/cpu-ops.h>
#include <asm/page.h>
#include <asm/pt.h>
#include <xen/mm.h>

/* Clear a pte. */
#define PTE_CLEAR(pte)    ((pte) = 0)

/*
 * Whatever context we run in, PDEs and PTEs are easy to access.
 * NOTE: CURRENT_PTE is only valid if the PDE type is PDE_TYPE_PT/
 */
#define CURRENT_PDE(va) (((pd_entry_t *) SPECIAL_PAGE_DIR)[PDE_INDEX(va)])
#define CURRENT_PTE(va) (((pt_entry_t *) PAGE_TABLE_VIRT_START)[(va) / PAGE_SIZE])
#define CURRENT_PT_PAGE(va) ROUND_DOWN(((vaddr_t) CURRENT_PTE(va)), PAGE_SIZE)

/*
 * Memory is allocated in PAGE_SIZE chunks, which is larger than
 * PT_SIZE, hence we need a way to find the actual offset of the PT.
 */
#define CURRENT_PAGE_TABLE_OFFSET(va) ((PDE_INDEX(va) % PT_PER_PAGE) * PT_SIZE)

/*
 * Create a PDE entry, allocating a page table.
 */
static int pt_make_pt_pde(vaddr_t va, pd_entry_flags_t flags)
{
   struct page_info *page;
   pt_entry_t *pt;

   BUG_ON(PDE_TYPE(CURRENT_PDE(va) != PDE_TYPE_FAULT));

   page = pages_u_alloc1(NULL);
   if (page == NULL) {
      return -ENOMEM;
   }

   /* Make the page appear in the virtual linear page tables. */
   pt = &CURRENT_PTE(PAGE_TABLE_VIRT_START);
   pt[va / (PAGE_SIZE * PT_SIZE)] =
      page_to_phys(page) |
      PTE_ENTRY_HV;

   cpu_clean_cache_range((vaddr_t) pt,
                         (vaddr_t) pt);

   clear_page((void *) CURRENT_PT_PAGE(va));

   CURRENT_PDE(va) = (page_to_phys(page) +
                      CURRENT_PAGE_TABLE_OFFSET(va)) |
      PDE_TABLE_HV;

   cpu_clean_cache_range((vaddr_t) &CURRENT_PDE(va),
                         (vaddr_t) &CURRENT_PDE(va));

   return 0;
}


/*
 * Returns a pointer to a PTE for a VA.
 */
static pt_entry_t *pt_get_pte(vaddr_t va)
{
   unsigned i, end_search_ix;
   pd_entry_t *pde = &CURRENT_PDE(va);

   BUG_ON(PDE_TYPE(*pde) == PDE_TYPE_SN);

   /* Easy case - the page table is already mapped. */
   if (PDE_TYPE(*pde) == PDE_TYPE_PT) {
     return &CURRENT_PTE(va);
   }

   /*
    * If the page table is not mapped, the backing page
    * could already exist and be mapped.
    */
   end_search_ix = ROUND_DOWN(PDE_INDEX(va), PT_PER_PAGE);
   for (i = 0; i < end_search_ix; i++) {
      if (PDE_TYPE(CURRENT_PDE(va)) == PDE_TYPE_PT) {

         /*
          * One of our neighbors's PT would exist in the same page,
          * so the PTs are already present and can be accessed
          * using CURRENT_PTE.
          */
         return &CURRENT_PTE(va);
      }
   }

   return NULL;
}

/*
 * Maps [base, base + size) to [va, va + size) with flags.
 */
int pt_map(vaddr_t va, paddr_t base, size_t size, pt_entry_flags_t flags)
{
   int status;
   pt_entry_t *pte;
   pd_entry_flags_t pde_flags;
   vaddr_t va_end = va + size;

   BUG_ON(va & ~PAGE_MASK);
   BUG_ON(va_end & ~PAGE_MASK);

   BUG_ON(!flags);
   switch (flags) {
   case PTE_ENTRY_HV:
      pde_flags = PDE_TABLE_HV;
      break;
   case PTE_ENTRY_IO:
      pde_flags = PDE_TABLE_IO;
      break;
   default:
      BUG_ON(flags);
   }

   while (va < va_end) {
      pte = pt_get_pte(va);
      if (pte == NULL) {
         status = pt_make_pt_pde(va, pde_flags);
         if (status) {
            return status;
         }

         pte = pt_get_pte(va);
         BUG_ON(pte == NULL);
      }

      BUG_ON(PTE_TYPE(*pte) != PTE_TYPE_FAULT);

      *pte = (base & PTE_BASE_MASK) | flags;
      va += PAGE_SIZE;
   }

   cpu_clean_cache_range((vaddr_t) pt_get_pte(va),
                         (vaddr_t) pt_get_pte(va_end - 1));

   cpu_flush_tlb_range(va, va_end - 1);
   return 0;
}

/*
 * Initializes the boot page tables.
 * 1) Allocate a page table for the last special megabyte.
 * 2) Map the page directory at it's special location.
 * 3) Allocate page tables to back PAGE_TABLE_VIRT_START-END.
 *
 * This may seem long and in a need of refactoring, however
 * subsequent access to page tables becomes very simple.
 *
 * The point of PAGE_TABLE_VIRT_START-PAGE_TABLE_VIRT_END is
 * that we virtually lay out all the page tables consecutively.
 * This means, each page entry describes 4KB, we can get
 * an page table entry index for VA by dividing by PAGE_SIZE.
 * Cute, huh?
 */
void pt_init(vaddr_t initial_pd_va)
{
   unsigned i;
   vaddr_t spt_va;
   pt_entry_t *spt, *cpt;
   struct page_info *cpt_page;
   pd_entry_t *pd = (pd_entry_t *) initial_pd_va;

   /*
    * Chicken-and-egg: need a mapped page for the special
    * page which we need for MAP0 mapping other items.
    */
   spt_va = (vaddr_t) pages_m_alloc1();
   BUG_ON(spt_va == NULL);

   spt = (pt_entry_t *)
      (spt_va +
       CURRENT_PAGE_TABLE_OFFSET(SPECIAL_VIRT_START));

   clear_page((void *) spt_va);
   pd[PDE_INDEX(SPECIAL_VIRT_START)] =
      va_to_ma(spt) | PDE_TABLE_HV;

   cpu_clean_cache_range((vaddr_t) pd,
                         ((vaddr_t) pd) + PD_SIZE - 1);

   /*
    * Now allocate 4 page tables for the linear virtual PT region,
    * which ends up being just a page, which we temporarily map
    * and initialize. Also map all the PD pages at a known
    * location, SPECIAL_PAGE_DIR, where it can be always accessed.
    */

   cpt_page = pages_u_alloc1(NULL);
   BUG_ON(cpt_page == NULL);

   cpt = (pt_entry_t *) SPECIAL_MAP0;
   spt[PTE_INDEX((vaddr_t) cpt)] =
      page_to_phys(cpt_page) | PTE_ENTRY_HV;

   for (i = 0; i < PD_SIZE / PAGE_SIZE; i++) {
      spt[PTE_INDEX(SPECIAL_PAGE_DIR + PAGE_SIZE * i)] =
         va_to_ma(initial_pd_va + PAGE_SIZE * i) | PTE_ENTRY_HV;
   }

   cpu_clean_cache_range((vaddr_t) spt,
                         (vaddr_t) spt + PT_SIZE - 1);
   cpu_flush_tlb_entry((vaddr_t) cpt);
   cpu_flush_tlb_entry((vaddr_t) SPECIAL_PAGE_DIR);
   clear_page((void *) cpt);

   /*
    * Account for cpt_page. Since 'cpt' is still not visible in
    * the VA space, we need to divide the VA by PAGE_SIZE *
    * PT_SIZE.
    */
   cpt[PAGE_TABLE_VIRT_START / (PAGE_SIZE * PT_SIZE)] =
      page_to_phys(cpt_page) |
      PTE_ENTRY_HV;

   /*
    * Account the page used to map SPECIAL_VIRT_START.
    */
   cpt[SPECIAL_VIRT_START / (PAGE_SIZE * PT_SIZE)] =
      va_to_ma(spt_va) |
      PTE_ENTRY_HV;

   cpu_clean_cache_range((vaddr_t) cpt,
                         (vaddr_t) cpt + PAGE_SIZE - 1);

   /*
    * Insert the four PTs into the PD.
    */
   for (i = 0; i < PT_PER_PAGE; i++) {
      pd[PDE_INDEX(PAGE_TABLE_VIRT_START) + i] =
         (page_to_phys(cpt_page) + (PT_SIZE * i)) | PDE_TABLE_HV;
   }

   cpu_clean_cache_range((vaddr_t) pd,
                         ((vaddr_t) pd) + PD_SIZE - 1);

   cpu_flush_tlb_range(PAGE_TABLE_VIRT_START, PAGE_TABLE_VIRT_END - 1);

   /* Unmap SPECIAL_MAP0. */
   PTE_CLEAR(spt[PTE_INDEX((vaddr_t) cpt)]);
   cpu_clean_cache_range((vaddr_t) spt,
                         (vaddr_t) spt + PT_SIZE - 1);
   cpu_flush_tlb_entry((vaddr_t) cpt);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
