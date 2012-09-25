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
 */

#include <xen/kernel.h>
#include <asm/config.h>
#include <asm/cpu-ops.h>
#include <asm/page.h>
#include <asm/pt.h>
#include <xen/mm.h>

/* PTE manipulation. */
#define PTE_INDEX_SHIFT (12)
#define PTE_INDEX(va)   (((va) >> PTE_INDEX_SHIFT) & (PT_ENTRIES - 1))
#define PTE_TYPE(pte)   ((pte) & PTE_TYPE_MASK)
#define PTE_CLEAR(pte)  ((pte) = 0)

/* PDE manipulation. */
#define PDE_INDEX_SHIFT   (20)
#define PDE_INDEX(va)     (((va) >> PDE_INDEX_SHIFT) & (PD_ENTRIES - 1))
#define PDE_TYPE(pde)     ((pde) & PDE_TYPE_MASK)
#define PDE_CLEAR(pde)    ((pde) = 0)
#define PDE_PT_BASE_MASK  (0xFFFFFC00)
#define PDE_PT_BASE(pde)  ((pde) & PDE_PT_BASE_MASK)

/*
 * Whatever context we run in, PDEs and PTEs are easy to access.
 * NOTE: CURRENT_PTE is only valid if the PDE type is PDE_TYPE_PT/
 */
#define CURRENT_PDE_AT_IX(ix) (((pd_entry_t *) SPECIAL_PAGE_DIR)[ix])
#define CURRENT_PDE(va) CURRENT_PDE_AT_IX(PDE_INDEX(va))
#define CURRENT_PTE(va) (((pt_entry_t *) PAGE_TABLE_VIRT_START)[(va) / PAGE_SIZE])
#define CURRENT_PT_PAGE(va) ROUND_DOWN(((vaddr_t) &CURRENT_PTE(va)), PAGE_SIZE)

/*
 * Memory is allocated in PAGE_SIZE chunks, which is larger than
 * PT_SIZE, hence we need a way to find the actual offset of the PT.
 */
#define CURRENT_PAGE_TABLE_OFFSET(va) ((PDE_INDEX(va) % PT_PER_PAGE) * PT_SIZE)

/*
 * Create a PDE entry, allocating a page table. It is expected
 * that a PDE does not exist (no existing page table entry or
 * section mapping).
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

   /*
    * Make the page appear in the virtual linear page tables. The whole
    * page of course is mapped, although only the portion matching the PT
    * for this VA will be useful as the rest won't have PDEs created
    * until required (through a call to pt_map->ept_get_pte.).
    */
   pt = (pt_entry_t *) ROUND_DOWN((vaddr_t) &CURRENT_PTE(PAGE_TABLE_VIRT_START), PAGE_SIZE);
   pt[va / (PAGE_SIZE * PT_SIZE)] =
      page_to_phys(page) |
      PTE_ENTRY_HV;
   cpu_clean_cache_range((vaddr_t) &pt[va / (PAGE_SIZE * PT_SIZE)],
                         (vaddr_t) &pt[va / (PAGE_SIZE * PT_SIZE)]);

   cpu_flush_tlb_range(CURRENT_PT_PAGE(va),
                       CURRENT_PT_PAGE(va) + PAGE_SIZE - 1);

   /* Page is cleared when allocated. */
   clear_page((void *) CURRENT_PT_PAGE(va));
   cpu_clean_cache_range((vaddr_t) CURRENT_PT_PAGE(va),
                         (vaddr_t) CURRENT_PT_PAGE(va) + PAGE_SIZE - 1);

   /*
    * Update the PDE entry for the PT needed for this VA. The page
    * is used by "neighboring" PTs (at indices [ROUND_DOWN(PDE_INDEX(va)
    * PT_PER_PAGE), += PT_PER_PAGE).
    */
   CURRENT_PDE(va) = (page_to_phys(page) +
                      CURRENT_PAGE_TABLE_OFFSET(va)) |
      PDE_TABLE_HV;
   cpu_clean_cache_range((vaddr_t) &CURRENT_PDE(va),
                         (vaddr_t) &CURRENT_PDE(va));

   return 0;
}


/*
 * Returns a pointer to a PTE for a VA. If the VA refers to a
 * page table that has already allocated physical backing and presence
 * in the virtual linear PT region (because one of the neighbor PTs is
 * in the physical page already is inserted into the PD), then the
 * appropriate PDE entry is created.
 *
 * It is not valid to call in this path with an existing section
 * (through PDE) mapping that covers this VA.
 */
static pt_entry_t *pt_get_pte(vaddr_t va)
{
   unsigned i, end_search_ix;
   pd_entry_t *pde = &CURRENT_PDE(va);

   BUG_ON(PDE_TYPE(*pde) == PDE_TYPE_SN);

   /* Easy case - the page table for this VA is already in the PD. */
   if (PDE_TYPE(*pde) == PDE_TYPE_PT) {
     return &CURRENT_PTE(va);
   }

   /*
    * If the page table is not mapped, the backing page
    * could already exist and be mapped. Scan the neighbor
    * PDEs. If any PDE refers to a PT, all we need to do
    * is create our PDE for our chunk of the existing
    * backing page.
    */
   i =  ROUND_DOWN(PDE_INDEX(va), PT_PER_PAGE);
   end_search_ix = i + PT_PER_PAGE;

   for (i; i < end_search_ix; i++) {
      if (PDE_TYPE(CURRENT_PDE_AT_IX(i)) == PDE_TYPE_PT) {
         CURRENT_PDE(va) =
            (ROUND_DOWN(PDE_PT_BASE(CURRENT_PDE_AT_IX(i)), PAGE_SIZE) +
             CURRENT_PAGE_TABLE_OFFSET(va)) |
            PDE_TABLE_HV;
         cpu_clean_cache_range((vaddr_t) &CURRENT_PDE(va),
                               (vaddr_t) &CURRENT_PDE(va));

         return &CURRENT_PTE(va);
      }
   }

   /* Looks like we'll need to allocate a PT before going any further. */
   return NULL;
}


/*
 * Maps [base, base + size) to [va, va + size) with flags, in PAGE_SIZE
 * chunks into the current address space.
 *
 * It is not valid to call this with a valid mapping for any VAs
 * affected by the mapping.
 *
 * It is not valid to call in this path with an existing section
 * (through PDE) mapping that covers this VA. Potentially, we could
 * convert any existing section mapping to a page table mapping, but
 * given that the section mechanism is used to map large swaths
 * non-changing mappings (such as I/O ranges or the memory backing
 * the hypervisor data/text), there shouldn't be a need.
 *
 * FIXME: properly handle hypervisor mappings. Newly created PDEs need
 * to be also made in the idle page tables, and the page fault path
 * would create the appropriate virtual linear PT region mappings and
 * PD entries.
 */
int pt_map(vaddr_t va, paddr_t base, size_t size, pt_entry_flags_t flags)
{
   int status;
   pt_entry_t *pte;
   pd_entry_flags_t pde_flags;
   vaddr_t va_start = va;
   vaddr_t va_end = va + size;

   /*
    * This is a low-level interface, and I think it's appropriate for
    * the caller to understand exactly what's mapped where.
    */
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

   /* VAs are processed in PAGE_SIZE chunks. */
   while (va < va_end) {
      pte = pt_get_pte(va);
      if (pte == NULL) {

         /*
          * No PT exists, and it needs to be allocated and accounted
          * for in the PD.
          */
         status = pt_make_pt_pde(va, pde_flags);
         if (status) {
            return status;
         }

         pte = pt_get_pte(va);

         /* Obviously at this point we shouldn't fail. */
         BUG_ON(pte == NULL);
      }


      BUG_ON(PTE_TYPE(*pte) != PTE_TYPE_FAULT);

      *pte = (base & PTE_BASE_MASK) | flags;
      va += PAGE_SIZE;
      base += PAGE_SIZE;
   }

   cpu_clean_cache_range((vaddr_t) pt_get_pte(va_start),
                         (vaddr_t) pt_get_pte(va_end - 1));
   cpu_flush_tlb_range(va_start, va_end - 1);
   return 0;
}


/*
 * Unmaps [base, base + size) from the current address space.
 *
 * As mappings in the hypervisor (versus VM) region are shared,
 * unmapping hypervisor mappings removes them from all address
 * spaces.
 */
void pt_unmap(vaddr_t va, size_t size)
{
   pt_entry_t *pte;
   vaddr_t va_start = va;
   vaddr_t va_end = va + size;

   /*
    * This is a low-level interface, and I think it's appropriate for
    * the caller to understand exactly what's mapped where.
    */
   BUG_ON(va & ~PAGE_MASK);
   BUG_ON(va_end & ~PAGE_MASK);
   BUG_ON(PDE_TYPE(CURRENT_PDE(va)) != PDE_TYPE_PT);
   BUG_ON(PDE_TYPE(CURRENT_PDE(va_end - 1)) != PDE_TYPE_PT);

   pte = &CURRENT_PTE(va_start);
   while (pte < &CURRENT_PTE(va_end)) {
      BUG_ON(PTE_TYPE(*pte) != PTE_TYPE_PAGE);

      *pte = PTE_TYPE_FAULT;
      pte++;
   }

   cpu_clean_cache_range((vaddr_t) &CURRENT_PTE(va_start),
                         (vaddr_t) &CURRENT_PTE(va_end - 1));
   cpu_flush_tlb_range(va_start, va_end - 1);
}


/*
 * Initializes the boot page tables.
 * 1) Allocate a page table for the last special megabyte.
 * 2) Map the page directory at it's special location.
 * 3) Allocate page tables to back the virtual linear PT
 *    region, which greatly simplifies PT management, as
 *    all PTs are laid out consecutively and do need
 *    explicit per-page maps/unmaps to update.
 *
 * This may seem long and in a need of refactoring, however
 * subsequent access to page tables becomes very simple.
 */
void pt_init(vaddr_t initial_pd_va)
{
   unsigned i;
   vaddr_t spt_va;
   pt_entry_t *spt, *vlpt;
   struct page_info *vlpt_page;
   pd_entry_t *pd = (pd_entry_t *) initial_pd_va;

   /*
    * Chicken-and-egg: need a mapped page for the special
    * PT which we need for MAP0 mapping other items. We'll
    * never spt_va to access the page table for the region
    * at SPECIAL_VIRT_START (not that it should matter, given
    * we only support ARMv7 non-aliasing VIPT dcache and no
    * code is present in the page to worry about the possibly-VIVT
    * icache.
    */
   spt_va = (vaddr_t) pages_m_alloc1();
   BUG_ON(spt_va == 0);
   clear_page((void *) spt_va);

   /* The actual PT for the SPECIAL_VIRT_START region. */
   spt = (pt_entry_t *)
      (spt_va +
       CURRENT_PAGE_TABLE_OFFSET(SPECIAL_VIRT_START));

   /* Account for it in the PD. */
   pd[PDE_INDEX(SPECIAL_VIRT_START)] =
      va_to_ma(spt) | PDE_TABLE_HV;
   cpu_clean_cache_range((vaddr_t) &pd[PDE_INDEX(SPECIAL_VIRT_START)],
                         (vaddr_t) &pd[PDE_INDEX(SPECIAL_VIRT_START)]);

   /*
    * Now allocate 4 page tables to back the virtual linear PT region,
    * which ends up being just a physical page, which we temporarily map
    * and initialize.
    *
    * Since the page allocated is unmapped, we map it in while we
    * need to set up a manuall SPECIAL_MAP0 mapping.
    */
   vlpt_page = pages_u_alloc1(NULL);
   BUG_ON(vlpt_page == NULL);

   vlpt = (pt_entry_t *) SPECIAL_MAP0;
   spt[PTE_INDEX((vaddr_t) vlpt)] =
      page_to_phys(vlpt_page) | PTE_ENTRY_HV;

   /*
    * Also map all the PD pages at a known location, SPECIAL_PAGE_DIR,
    * where it can be always accessed.
    *
    * PD, of course, takes more than one mapping, being 4 * PAGE_SIZE.
    */
   for (i = 0; i < PD_SIZE / PAGE_SIZE; i++) {
      spt[PTE_INDEX(SPECIAL_PAGE_DIR + PAGE_SIZE * i)] =
         va_to_ma(initial_pd_va + PAGE_SIZE * i) | PTE_ENTRY_HV;
   }

   /* The whole range so the clear_page and all modifications stick. */
   cpu_clean_cache_range((vaddr_t) spt,
                         (vaddr_t) spt + PT_SIZE - 1);
   cpu_flush_tlb_entry((vaddr_t) vlpt);
   cpu_flush_tlb_range((vaddr_t) SPECIAL_VIRT_START,
                       (vaddr_t) SPECIAL_VIRT_LAST_VALID);

   /*
    * Account for vlpt_page. Since 'vlpt' is still not visible in
    * the virtual linear PT space, we need to divide the VA by
    * PAGE_SIZE * PT_SIZE. This will allow us to get at the
    * page tables backing the virtual linear PT space, so
    * we can account for future allocated PTs later, so they're
    * always mapped in the VLPT region and accesible for PTE
    * accesses.
    */
   clear_page((void *) vlpt);
   vlpt[PAGE_TABLE_VIRT_START / (PAGE_SIZE * PT_SIZE)] =
      page_to_phys(vlpt_page) |
      PTE_ENTRY_HV;

   /* Describe the page backing the PT for SPECIAL_VIRT_START. */
   vlpt[SPECIAL_VIRT_START / (PAGE_SIZE * PT_SIZE)] =
      va_to_ma(spt_va) |
      PTE_ENTRY_HV;

   /* The whole range so the clear_page and all modifications stick. */
   cpu_clean_cache_range((vaddr_t) vlpt,
                         (vaddr_t) vlpt + PAGE_SIZE - 1);

   /* Insert the four PTs into the PD. */
   for (i = 0; i < PT_PER_PAGE; i++) {
      pd[PDE_INDEX(PAGE_TABLE_VIRT_START) + i] =
         (page_to_phys(vlpt_page) + (PT_SIZE * i)) | PDE_TABLE_HV;
   }
   cpu_clean_cache_range((vaddr_t) &pd[PDE_INDEX(PAGE_TABLE_VIRT_START)],
                         (vaddr_t) &pd[PDE_INDEX(PAGE_TABLE_VIRT_START) + PT_PER_PAGE]);
   cpu_flush_tlb_range(PAGE_TABLE_VIRT_START, PAGE_TABLE_VIRT_END - 1);

   /* Unmap SPECIAL_MAP0. */
   PTE_CLEAR(spt[PTE_INDEX((vaddr_t) vlpt)]);
   cpu_clean_cache_range((vaddr_t) spt,
                         (vaddr_t) spt + PT_SIZE - 1);
   cpu_flush_tlb_entry((vaddr_t) vlpt);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
