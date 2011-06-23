/*
 *  linux/arch/arm/mm/tlb-v6.S
 *
 *  Copyright (C) 1997-2002 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  ARM architecture version 6 TLB handling functions.
 *  These assume a split I/D TLB.
 */
#include <xen/config.h>
#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>

#define HARVARD_TLB
#define PAGE_SZ 4096 /* PAGE_SIZE	@ */

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_tlb_all)
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mcr	p15, 0, r0, c8, c6,  0		@ invalidate Entire I TLB
	mcr	p15, 0, r0, c8, c5,  0		@ invalidate Entire D TLB
	mcr p15, 0, r0, c7, c5, 6
	mcr	p15, 0, r0, c7, c10, 4		@ data synchronization barrier
	mcr	p15, 0, r0, c7, c5, 4		@ prefetch flush
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_all, v6_flush_tlb_all)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_tlb_entry)
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 4		@ drain write buffer
	mov	r0, r0, lsr #PAGE_SHIFT		@ align address
	mov	r1, r1, lsr #PAGE_SHIFT
	mov	r0, r0, lsl #PAGE_SHIFT
	mov	r1, r1, lsl #PAGE_SHIFT
#ifdef HARVARD_TLB
	mcr	p15, 0, r0, c8, c6, 1		@ TLB invalidate D MVA
	mcr	p15, 0, r0, c8, c5, 1		@ TLB invalidate I MVA
#else
	mcr	p15, 0, r0, c8, c7, 1		@ TLB invalidate MVA
#endif
	mcr	p15, 0, r2, c7, c5, 6		@ flush BTAC/BTB
	mcr	p15, 0, r2, c7, c10, 4		@ data synchronization barrier
	mcr	p15, 0, r2, c7, c5, 4		@ prefetch flush
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_entry, v6_flush_tlb_entry)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_tlb_range)
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 4		@ drain write buffer
	mov	r0, r0, lsr #PAGE_SHIFT		@ align address
	mov	r1, r1, lsr #PAGE_SHIFT
	mov	r0, r0, lsl #PAGE_SHIFT
	mov	r1, r1, lsl #PAGE_SHIFT
1:
#ifdef HARVARD_TLB
	mcr	p15, 0, r0, c8, c6, 1		@ TLB invalidate D MVA
	mcr	p15, 0, r0, c8, c5, 1		@ TLB invalidate I MVA
#else
	mcr	p15, 0, r0, c8, c7, 1		@ TLB invalidate MVA
#endif
	add	r0, r0, #PAGE_SZ
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r2, c7, c5, 6		@ flush BTAC/BTB
	mcr	p15, 0, r2, c7, c10, 4		@ data synchronization barrier
	mcr	p15, 0, r2, c7, c5, 4		@ prefetch flush
	mov	pc, lr
	
DECLARE_CPU_OP(cpu_flush_tlb_range, v6_flush_tlb_range)


