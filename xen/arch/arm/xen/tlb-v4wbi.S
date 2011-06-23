/*
 *  linux/arch/arm/mm/tlbv4wbi.S
 *
 *  Copyright (C) 1997-2002 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  ARM architecture version 4 and version 5 TLB handling functions.
 *  These assume a split I/D TLBs, with a write buffer.
 *
 *  Processors: ARM920 ARM922 ARM925 ARM926 XScale
 */

#include <xen/config.h>
#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v4wbi_flush_tlb_all)
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mcr	p15, 0, r0, c8, c6,  0		@ invalidate Entire I TLB
	mcr	p15, 0, r0, c8, c5,  0		@ invalidate Entire D TLB
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_all, v4wbi_flush_tlb_all)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v4wbi_flush_tlb_entry)
	mov	r3, #0
        mcr     p15, 0, r3, c7, c10, 4          @ drain WB
	bic	r0, r0, #0x0ff
	bic	r0, r0, #0xf00
    mcr     p15, 0, r0, c8, c6, 1           @ invalidate D TLB entry
	mcr     p15, 0, r0, c8, c5, 1           @ invalidate I TLB entry
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_entry, v4wbi_flush_tlb_entry)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v4wbi_flush_tlb_range)
	mov     r3, #0
	mcr     p15, 0, r3, c7, c10, 4          @ drain WB
	bic     r0, r0, #0x0ff
	bic     r0, r0, #0xf00
1: 
	mcr     p15, 0, r0, c8, c6, 1           @ invalidate D TLB entry
	mcr     p15, 0, r0, c8, c5, 1           @ invalidate I TLB entry
	add     r0, r0, #PAGE_SIZE
	cmp     r0, r1
	blo     1b
	mov     pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_range, v4wbi_flush_tlb_range)


