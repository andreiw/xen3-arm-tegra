/*
 *  linux/arch/arm/mm/proc-v7.S
 *
 *  Copyright (C) 2001 Deep Blue Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This is the "shell" of the ARMv7 processor support.
 */

#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>
#include <asm/system.h>
#include <asm/asm-offsets.h>
#include <asm/arch/entry-macro.S>

.text

#define TTB_S		(1 << 1)
#define TTB_RGN_NC	(0 << 3)
#define TTB_RGN_OC_WBWA	(1 << 3)
#define TTB_RGN_OC_WT	(2 << 3)
#define TTB_RGN_OC_WB	(3 << 3)
#define TTB_NOS		(1 << 5)
#define TTB_IRGN_NC	((0 << 0) | (0 << 6))
#define TTB_IRGN_WBWA	((0 << 0) | (1 << 6))
#define TTB_IRGN_WT	((1 << 0) | (0 << 6))
#define TTB_IRGN_WB	((1 << 0) | (1 << 6))

/* PTWs cacheable, inner WB not shareable, outer WB not shareable */
#define TTB_FLAGS_UP	TTB_IRGN_WB|TTB_RGN_OC_WB
#define PMD_FLAGS_UP	PMD_SECT_WB

/* PTWs cacheable, inner WBWA shareable, outer WBWA not shareable */
#define TTB_FLAGS_SMP	TTB_IRGN_WBWA|TTB_S|TTB_NOS|TTB_RGN_OC_WBWA
#define PMD_FLAGS_SMP	PMD_SECT_WBWA|PMD_SECT_S

/*
 * dcache_line_size - get the minimum D-cache line size from the CTR register
 * on ARMv7.
 */
	.macro	dcache_line_size, reg, tmp
	mrc	p15, 0, \tmp, c0, c0, 1		@ read ctr
	lsr	\tmp, \tmp, #16
	and	\tmp, \tmp, #0xf		@ cache line size encoding
	mov	\reg, #4			@ bytes per word
	mov	\reg, \reg, lsl \tmp		@ actual cache line size
	.endm

/*
 * icache_line_size - get the minimum I-cache line size from the CTR register
 * on ARMv7.
 */
	.macro	icache_line_size, reg, tmp
	mrc	p15, 0, \tmp, c0, c0, 1		@ read ctr
	and	\tmp, \tmp, #0xf		@ cache line size encoding
	mov	\reg, #4			@ bytes per word
	mov	\reg, \reg, lsl \tmp		@ actual cache line size
	.endm

PRIVATE(v7_idle)
	dsb					@ WFI may enter a low-power mode
	wfi
	mov	pc, lr
DECLARE_CPU_OP(cpu_idle, v7_idle)

PRIVATE(v7_halt)
	stmfd	sp!, {lr}
	cpsid	if				@ disable interrupts
	mrc	p15, 0, r0, c1, c0, 0		@ ctrl register
	bic	r0, r0, #0x1000			@ ...i............
	bic	r0, r0, #0x0006			@ .............ca.
	mcr	p15, 0, r0, c1, c0, 0		@ disable caches
	ldmfd	sp!, {pc}
DECLARE_CPU_OP(cpu_halt, v7_idle)

PRIVATE(v7_reset)
	mov	pc, r0
DECLARE_CPU_OP(cpu_reset, v7_reset)

PRIVATE(v7_get_ttb)
	mrc	p15, 0, r0, c2, c0, 0		@ Get TTB0.
	mov	pc, lr
DECLARE_CPU_OP(cpu_get_ttb, v7_get_ttb)

PRIVATE(v7_switch_ttb)
	mov	r2, #0
	orr	r0, r0, #TTB_FLAGS_UP
#ifdef CONFIG_ARM_ERRATA_430973
	mcr	p15, 0, r2, c7, c5, 6		@ flush BTAC/BTB
#endif
#ifdef CONFIG_ARM_ERRATA_754322
	dsb
#endif
	mcr	p15, 0, r0, c2, c0, 0		@ set TTB 0
	isb
#ifdef CONFIG_ARM_ERRATA_754322
	dsb
#endif
	isb
	mov	pc, lr
DECLARE_CPU_OP(cpu_switch_ttb, v7_switch_ttb)

PRIVATE(v7_flush_dcache_all)
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	ands	r3, r0, #0x7000000		@ extract loc from clidr
	mov	r3, r3, lsr #23			@ left align loc bit field
	beq	finished			@ if loc is 0, then no need to clean
	mov	r10, #0				@ start clean at cache level 0
loop1:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	skip				@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
loop2:
	mov	r9, r4				@ create working copy of max way size
loop3:
	orr	r11, r10, r9, lsl r5		@ factor way and cache number into r11
	orr	r11, r11, r7, lsl r2		@ factor index number into r11
	mcr	p15, 0, r11, c7, c14, 2		@ clean & invalidate by set/way
	subs	r9, r9, #1			@ decrement the way
	bge	loop3
	subs	r7, r7, #1			@ decrement the index
	bge	loop2
skip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	loop1
finished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb
	isb
	mov	pc, lr

PRIVATE(v7_flush_kern_cache_all)
	stmfd	sp!, {r4-r5, r7, r9-r11, lr}
	bl	v7_flush_dcache_all
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0	@ I+BTB cache invalidate
	ldmfd	sp!, {r4-r5, r7, r9-r11, lr}
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_cache_all, v7_flush_kern_cache_all)

PRIVATE(v7_flush_cache_page)
	dcache_line_size r2, r3
	add	r1, r0, #PAGE_SIZE
1:	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D line / unified line
	add	r0, r0, r2
	cmp	r0, r1
	blo	 1b
	dsb
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_cache_page, v7_flush_cache_page)

PRIVATE(v7_flush_cache_range)
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_cache_range, v7_flush_cache_range)

PRIVATE(v7_clean_cache_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:	mcr	p15, 0, r0, c7, c10, 1	@ clean D entry
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
DECLARE_CPU_OP(cpu_clean_cache_range, v7_clean_cache_range)

PRIVATE(v7_flush_dcache_entry)
	mcr	p15, 0, r0, c7, c10, 1
	dsb
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_cache_entry, v7_flush_dcache_entry)

	/*
 *	v7_dma_inv_range(start,end)
 *
 *	Invalidate the data cache within the specified region; we will
 *	be performing a DMA operation in this region and we want to
 *	purge old data in the cache.
 *
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
PRIVATE(v7_dma_inv_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	tst	r0, r3
	bic	r0, r0, r3
	mcrne	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line

	tst	r1, r3
	bic	r1, r1, r3
	mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D / U line
1:
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
DECLARE_CPU_OP(cpu_invalidate_dma_range, v7_dma_inv_range)

/*
 *	v7_dma_clean_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
PRIVATE(v7_dma_clean_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c10, 1		@ clean D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
DECLARE_CPU_OP(cpu_clean_dma_range, v7_dma_clean_range)

PRIVATE(v7_dma_flush_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
DECLARE_CPU_OP(cpu_flush_dma_range, v7_dma_flush_range)

PRIVATE(v7_coherent_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r12, r0, r3
1:	mcr	p15, 0, r12, c7, c11, 1	@ Clean D line to the point of unification.
	add	r12, r12, r2
	cmp	r12, r1
	blo	1b
	dsb
	icache_line_size r2, r3
	sub	r3, r2, #1
	bic	r12, r0, r3
2:	mcr	p15, 0, r12, c7, c5, 1	@ Invalidate I line
	add	r12, r12, r2
	cmp	r12, r1
	blo	2b
3:	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BTB
	dsb
	isb
	mov	pc, lr
DECLARE_CPU_OP (cpu_coherent_range, v7_coherent_range)