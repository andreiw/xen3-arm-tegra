/*
 *  linux/arch/arm/mm/tlb-v7.S
 *
 *  Copyright (C) 1997-2002 Russell King
 *  Modified for ARMv7 by Catalin Marinas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  ARM architecture version 7 TLB handling functions.
 *  These assume a split I/D TLB.
 */

#include <xen/config.h>
#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>

PRIVATE(v7_flush_tlb_all)
	dsb
	mcr	p15, 0, r0, c8, c7, 0 @ TLB invaludate U all
	mcr	p15, 0, ip, c7, c5, 6 @ Flush BTAC/BTB
	dsb
	isb
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_tlb_all, v7_flush_tlb_all)

PRIVATE(v7_flush_tlb_entry)
	mov	r2, #0
	dsb
	mov	r0, r0, lsr #PAGE_SHIFT		@ align address
	mov	r1, r1, lsr #PAGE_SHIFT
	mov	r0, r0, lsl #PAGE_SHIFT
	mov	r1, r1, lsl #PAGE_SHIFT
	mcr	p15, 0, r0, c8, c7, 1		@ TLB invalidate U MVA
	mcr	p15, 0, r2, c7, c5, 6		@ flush BTAC/BTB
	dsb
	isb
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_tlb_entry, v7_flush_tlb_entry)

PRIVATE(v7_flush_tlb_range)
	mov	r2, #0
	dsb
	mov	r0, r0, lsr #PAGE_SHIFT		@ align address
	mov	r1, r1, lsr #PAGE_SHIFT
	mov	r0, r0, lsl #PAGE_SHIFT
	mov	r1, r1, lsl #PAGE_SHIFT
1:	mcr	p15, 0, r0, c8, c7, 1		@ TLB invalidate U MVA
	add	r0, r0, #PAGE_SIZE
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r2, c7, c5, 6		@ flush BTAC/BTB
	dsb
	isb
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_tlb_range, v7_flush_tlb_range)
