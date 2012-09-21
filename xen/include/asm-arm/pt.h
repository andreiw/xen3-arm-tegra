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

#ifndef XEN_ARM_PT_H
#define XEN_ARM_PT_H

#include <xen/sizes.h>
#include <asm/cpu-domain.h>

/* PDE definition. */
#define PDE_TYPE_MASK     (0x3)
#define PDE_TYPE_FAULT    (0)
#define PDE_TYPE_PT       (1) /* Second level table. */
#define PDE_TYPE_SN       (2) /* Section. */
#define PDE_TYPE(pde)     ((pde) & PDE_TYPE_MASK)
#define PDE_DOMAIN_SHIFT  (5)
#define PDE_DOMAIN_MASK   (0xF)
#define PDE_DOMAIN(dom)   ((dom) << PDE_DOMAIN_SHIFT)
#define PDE_IMP           (1 << 9)
#define PDE_PT_BASE_MASK  (0xFFFFFC00)
#define PDE_SN_B          (1 << 2)
#define PDE_SN_C          (1 << 3)
#define PDE_SN_XN         (1 << 4)
#define PDE_ACCESS        (1 << 10)
#define PDE_SN_AP1        (1 << 11)
#define PDE_SN_AP2        (1 << 15)
#define PDE_P_RW          (0)                       /* Priviledged RW */
#define PDE_U_RW          (PDE_SN_AP1)              /* Unpriviledged RW */
#define PDE_P_RO          (PDE_SN_AP2)              /* Priviledged RO */
#define PDE_U_RO          (PDE_SN_AP1 | PDE_SN_AP2) /* Unpriviledged RO */
#define PDE_SN_TEX_SHIFT  (12)
#define PDE_SN_TEX_MASK   (0x7)
#define PDE_SN_TEX(tex)   ((tex) << PDE_SN_TEX_SHIFT)
#define PDE_SN_S          (1 << 16)
#define PDE_SN_NG         (1 << 17)
#define PDE_SN_BASE_MASK  (0xFFF00000)

/*
 * Hypervisor mappings at boot time:
 * 1MB sections, outer and inner write-back, no write-allocate,
 * global, and the access flag is set. Access is priviledged RW.
 */
#define PDE_SECTION_HV      (PDE_TYPE_SN | PDE_DOMAIN(DOMAIN_HYPERVISOR) | PDE_SN_C | PDE_SN_B | PDE_ACCESS | PDE_P_RW)
#define PDE_TABLE_HV        (PDE_TYPE_PT | PDE_DOMAIN(DOMAIN_HYPERVISOR))
#define PTE_ENTRY_HV        (PTE_TYPE_PAGE | PTE_C | PTE_B | PTE_ACCESS | PTE_P_RW)

/*
 * I/O mappings at boot time:
 * Device-ordered, shareable, global, and the access flag is set.
 * Access is priviledged RW.
 */
#define PDE_SECTION_IO     (PDE_TYPE_SN | PDE_DOMAIN(DOMAIN_IO) | PDE_SN_B | PDE_ACCESS | PDE_P_RW)
#define PDE_TABLE_IO       (PDE_TYPE_PT | PDE_DOMAIN(DOMAIN_IO))
#define PTE_ENTRY_IO        (PTE_TYPE_PAGE | PTE_ACCESS)

/* PD size. */
#define PD_SIZE           (SZ_16K)
#define PD_ENTRIES        (4096)

/* Used to calculate a page directory entry index. */
#define PDE_INDEX_SHIFT   (20)
#define PDE_INDEX(va)     (((va) >> PDE_INDEX_SHIFT) & (PD_ENTRIES - 1))

/*
 * Other than regular PAGE_SIZE, we can also map
 * 1MB chunks directly with the page directory.
 */

#define PDE_SECTION_SHIFT (20)
#define PDE_SECTION_SIZE  (1 << PDE_SECTION_SHIFT)
#define PDE_SECTION_MASK  (~(PDE_SECTION_SIZE) - 1)

/* PTE definition. */
#define PTE_TYPE_MASK     (0x3)
#define PTE_TYPE_FAULT    (0x0)
#define PTE_TYPE_PAGE     (0x2)
#define PTE_TYPE(pte)     ((pte) & PTE_TYPE_MASK)
#define PTE_XN            (1 << 1)
#define PTE_B             (1 << 2)
#define PTE_C             (1 << 3)
#define PTE_ACCESS        (1 << 4)
#define PTE_AP1           (1 << 5)
#define PTE_AP2           (1 << 9)
#define PTE_P_RW          (0)                  /* Priviledged  RW */
#define PTE_U_RW          (PTE_AP1)            /* Unpriviledged RW */
#define PTE_P_RO          (PTE_AP2)            /* Priviledged RO */
#define PTE_U_RO          (PTE_AP1 | PTE_AP2)  /* Unpriviledged RO */
#define PTE_TEX_SHIFT     (6)
#define PTE_TEX_MASK      (0x7)
#define PTE_S             (1 << 10)
#define PTE_NG            (1 << 11)
#define PTE_BASE_MASK     (0xFFFFF000)

/* PT size. */
#define PT_SIZE           (SZ_1K)
#define PT_ENTRIES        (256)
#define PT_PER_PAGE       (PAGE_SIZE / PT_SIZE)

/* Used to calculate a page table entry index. */
#define PTE_INDEX_SHIFT   (12)
#define PTE_INDEX(va)     (((va) >> PTE_INDEX_SHIFT) & (PT_ENTRIES - 1))

#ifndef __ASSEMBLY__

/* A page directory entry is 4 bytes wide. */
typedef u32 pd_entry_t;
typedef u32 pd_entry_flags_t;

/* Clear a pde. */
#define PDE_CLEAR(pde)    ((pde) = 0)

/* A page table entry is 4 bytes wide. */
typedef u32 pt_entry_t;
typedef u32 pt_entry_flags_t;

/* Initialize page table support. */
void pt_init(vaddr_t initial_pd_va);
int pt_map(vaddr_t va, paddr_t base, size_t size, pt_entry_flags_t flags);

#endif /* __ASSEMBLY__ */
#endif /* XEN_ARM_PT_H */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
