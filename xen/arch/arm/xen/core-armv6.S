/*
 *  linux/arch/arm/mm/proc-v6.S
 *
 *  Copyright (C) 2001 Deep Blue Solutions Ltd.
 *  Modified by Catalin Marinas for noMMU support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This is the "shell" of the ARMv6 processor support.
 */

#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>
#include <asm/system.h>
#include <asm/asm-offsets.h>
#include <asm/arch/entry-macro.S>

#define HARVARD_CACHE
#define CACHE_LINE_SIZE		32
#define D_CACHE_LINE_SIZE	32
#define BTB_FLUSH_SIZE		8
#define PAGE_SZ				4096

#define TTB_C				(1 << 0)
#define TTB_S				(1 << 1)
#define TTB_IMP				(1 << 2)
#define TTB_RGN_NC			(0 << 3)
#define TTB_RGN_WBWA		(1 << 3)
#define TTB_RGN_WT			(2 << 3)
#define TTB_RGN_WB			(3 << 3)

#define TTB_FLAGS			TTB_RGN_WBWA

	.text
/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_idle)
#ifndef CONFIG_NOT_USE_WFI
		mcr		p15, 0, r1, c7, c0, 4		@ wait for interrupt
#else
		wait_for_interrupt	r1, r2		@ arch-defined opreration
#endif
		mov		pc, lr

DECLARE_CPU_OP(cpu_idle, v6_idle)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_halt)
		stmfd	sp!, {lr}
		cpsid	if							@ disable interrupts
		bl		v6_flush_kern_cache_all
		mrc		p15, 0, r0, c1, c0, 0		@ ctrl register
		bic		r0, r0, #0x1000				@ ...i............
		bic		r0, r0, #0x0006				@ .............ca.
		mcr		p15, 0, r0, c1, c0, 0		@ disable caches
		ldmfd	sp!, {pc}

DECLARE_CPU_OP(cpu_halt, v6_halt)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_reset)
		mov		pc, r0

DECLARE_CPU_OP(cpu_reset, v6_reset)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_get_ttb)
		mrc		p15, 0, r0, c2, c0, 0		@ get the current TTB
		mov		pc, lr

DECLARE_CPU_OP(cpu_get_ttb, v6_get_ttb)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_switch_mm)
		mov		r2, #0
		mcr		p15, 0, r2, c7, c10, 4		@ drain WB
		mcr		p15, 0, r2, c8, c6,  0		@ invalidate Entire I TLB
		mcr		p15, 0, r2, c8, c5,  0		@ invalidate Entire D TLB
		mcr 	p15, 0, r2, c7, c5, 6
		mcr		p15, 0, r2, c7, c10, 4		@ data synchronization barrier
		mcr		p15, 0, r2, c7, c5, 4		@ prefetch flush

//		ldr		r1, [r1, #OFFSET_ASID]	@ get mm->context.id
//		orr		r0, r0, #TTB_FLAGS
		mcr		p15, 0, r2, c7, c5, 6		@ flush BTAC/BTB
		mcr		p15, 0, r2, c7, c10, 4		@ drain write buffer
		mcr		p15, 0, r0, c2, c0, 0		@ set TTB 0
//		mcr		p15, 0, r1, c13, c0, 1		@ set context ID
		mov		pc, lr

DECLARE_CPU_OP(cpu_switch_ttb, v6_switch_mm)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_kern_cache_all)
#if 1
		mov		r0, #0
#ifdef HARVARD_CACHE
		mcr		p15, 0, r0, c7, c14, 0		@ D cache clean+invalidate
		mcr		p15, 0, r0, c7, c5, 0		@ I+BTB cache invalidate
#else
		mcr		p15, 0, r0, c7, c15, 0		@ Cache clean+invalidate
#endif
#endif
		mcr		p15, 0, r0, c7, c10, 4
		mov		pc, lr

DECLARE_CPU_OP(cpu_flush_cache_all, v6_flush_kern_cache_all)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_kern_dcache_page)
#if 1
		add		r1, r0, #PAGE_SZ
1:
#ifdef HARVARD_CACHE
		mcr		p15, 0, r0, c7, c14, 1		@ clean & invalidate D line
#else
		mcr		p15, 0, r0, c7, c15, 1		@ clean & invalidate unified line
#endif	
		add		r0, r0, #D_CACHE_LINE_SIZE
		cmp		r0, r1
		blo		1b
#ifdef HARVARD_CACHE
		mov		r0, #0
		mcr		p15, 0, r0, c7, c10, 4
#endif
#endif
		mov		pc, lr

DECLARE_CPU_OP(cpu_flush_cache_page, v6_flush_kern_dcache_page)

/*---------------------------------------------------------------------------
 *
 *	v6_flush_cache_range(start, end, flags)
 *
 *	Flush a range of TLB entries in the specified address space.
 *
 *	- start - start address (may not be aligned)
 *	- end   - end address (exclusive, may not be aligned)
 *	- flags	- vm_area_struct flags describing address space
 *
 *	It is assumed that:
 *	- we have a VIPT cache.
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_user_cache_range)
		mov		pc, lr

DECLARE_CPU_OP(cpu_flush_cache_range, v6_flush_user_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_clean_cache_range)
#ifndef TLB_CAN_READ_FROM_L1_CACHE
1:
		mcr		p15, 0, r0, c7, c10, 1		@ clean D entry
		add		r0, r0, #D_CACHE_LINE_SIZE
		subs	r1, r1, #D_CACHE_LINE_SIZE
		bhi		1b
#endif
		mov		pc, lr

DECLARE_CPU_OP(cpu_clean_cache_range, v6_clean_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v6_flush_dcache_entry)
        mcr     p15, 0, r0, c7, c10, 1
        mov     r0, #0
        mcr     p15, 0, r0, c7, c10, 4
        mov     pc, lr

DECLARE_CPU_OP(cpu_flush_cache_entry, v6_flush_dcache_entry)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v6_dma_inv_range)
		tst		r0, #D_CACHE_LINE_SIZE - 1
		bic		r0, r0, #D_CACHE_LINE_SIZE - 1
#ifdef HARVARD_CACHE
		mcrne	p15, 0, r0, c7, c10, 1		@ clean D line
#else
		mcrne	p15, 0, r0, c7, c11, 1		@ clean unified line
#endif
		tst		r1, #D_CACHE_LINE_SIZE - 1
		bic		r1, r1, #D_CACHE_LINE_SIZE - 1
#ifdef HARVARD_CACHE
		mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D line
#else
		mcrne	p15, 0, r1, c7, c15, 1		@ clean & invalidate unified line
#endif
1:
#ifdef HARVARD_CACHE
		mcr		p15, 0, r0, c7, c6, 1		@ invalidate D line
#else
		mcr		p15, 0, r0, c7, c7, 1		@ invalidate unified line
#endif
		add		r0, r0, #D_CACHE_LINE_SIZE
		cmp		r0, r1
		blo		1b
		mov		r0, #0
		mcr		p15, 0, r0, c7, c10, 4		@ drain write buffer
		mov		pc, lr
	
DECLARE_CPU_OP(cpu_invalidate_dma_range, v6_dma_inv_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v6_dma_clean_range)
		bic		r0, r0, #D_CACHE_LINE_SIZE - 1
1:
#ifdef HARVARD_CACHE
		mcr		p15, 0, r0, c7, c10, 1		@ clean D line
#else
		mcr		p15, 0, r0, c7, c11, 1		@ clean unified line
#endif
		add		r0, r0, #D_CACHE_LINE_SIZE
		cmp		r0, r1
		blo		1b
		mov		r0, #0
		mcr		p15, 0, r0, c7, c10, 4		@ drain write buffer
		mov		pc, lr
	
DECLARE_CPU_OP(cpu_clean_dma_range, v6_dma_clean_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v6_dma_flush_range)
		bic		r0, r0, #D_CACHE_LINE_SIZE - 1
1:
#ifdef HARVARD_CACHE
		mcr		p15, 0, r0, c7, c14, 1		@ clean & invalidate D line
#else
		mcr		p15, 0, r0, c7, c15, 1		@ clean & invalidate line
#endif
		add		r0, r0, #D_CACHE_LINE_SIZE
		cmp		r0, r1
		blo		1b
		mov		r0, #0
		mcr		p15, 0, r0, c7, c10, 4		@ drain write buffer
		mov		pc, lr
	
DECLARE_CPU_OP(cpu_flush_dma_range, v6_dma_flush_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v6_coherent_kern_range)
	/* FALLTHROUGH */

ENTRY(v6_coherent_user_range)
#ifdef HARVARD_CACHE
		bic		r0, r0, #CACHE_LINE_SIZE - 1
1:
		mcr		p15, 0, r0, c7, c10, 1		@ clean D line
		add		r0, r0, #CACHE_LINE_SIZE
		cmp		r0, r1
		blo		1b
#endif
		mov		r0, #0
#ifdef HARVARD_CACHE
		mcr		p15, 0, r0, c7, c10, 4		@ drain write buffer
		mcr		p15, 0, r0, c7, c5, 0		@ I+BTB cache invalidate
#else
		mcr		p15, 0, r0, c7, c5, 6		@ invalidate BTB
#endif
		mov		pc, lr

DECLARE_CPU_OP(cpu_coherent_range, v6_coherent_user_range)
