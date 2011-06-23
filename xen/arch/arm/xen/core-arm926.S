/*
 * core-arm926.S
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
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

#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>
#include <asm/system.h>

#define VM_EXEC     0x00000004

#define CACHE_DLIMIT	16384

#define CACHE_DLINESIZE	32

	.text
/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_idle)
	mov	r0, #0
	mrc	p15, 0, r1, c1, c0, 0		@ Read control register
	mcr	p15, 0, r0, c7, c10, 4		@ Drain write buffer
	bic	r2, r1, #1 << 12
	mcr	p15, 0, r2, c1, c0, 0		@ Disable I cache
	mcr	p15, 0, r0, c7, c0, 4		@ Wait for interrupt
	mcr	p15, 0, r1, c1, c0, 0		@ Restore ICache enable
	mov	pc, lr

DECLARE_CPU_OP(cpu_idle, arm926_idle)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_halt)
	stmfd	sp!, {lr}
	mov	ip, #PSR_F_BIT | PSR_I_BIT | PSR_MODE_SVC
	msr	cpsr_c, ip
        bl      arm926_flush_kern_cache_all     @ clean caches
	mrc	p15, 0, r0, c1, c0, 0		@ ctrl register
	bic	r0, r0, #0x1000			@ ...i............
	bic	r0, r0, #0x000e			@ ............wca.
	mcr	p15, 0, r0, c1, c0, 0		@ disable caches
	ldmfd	sp!, {pc}

DECLARE_CPU_OP(cpu_halt, arm926_halt)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_reset)
	mov	ip, #0
	mcr	p15, 0, ip, c7, c7, 0		@ invalidate I,D caches
	mcr	p15, 0, ip, c7, c10, 4		@ drain WB
	mcr	p15, 0, ip, c8, c7, 0		@ invalidate I & D TLBs
	mrc	p15, 0, ip, c1, c0, 0		@ ctrl register
	bic	ip, ip, #0x000f			@ ............wcam
	bic	ip, ip, #0x1100			@ ...i...s........
	mcr	p15, 0, ip, c1, c0, 0		@ ctrl register
	mov	pc, r0

DECLARE_CPU_OP(cpu_reset, arm926_reset)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_get_ttb)
	mrc	p15, 0, r0, c2, c0, 0		@ get the current TTB
	mov	pc, lr

DECLARE_CPU_OP(cpu_get_ttb, arm926_get_ttb)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_switch_mm)
	mov	ip, #0
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, ip, c7, c6, 0		@ invalidate D cache
#else
@ && 'Clean & Invalidate whole DCache'
1:	mrc	p15, 0, r15, c7, c14, 3 	@ test,clean,invalidate
	bne	1b
#endif
	mcr	p15, 0, ip, c7, c5, 0		@ invalidate I cache
	mcr	p15, 0, ip, c7, c10, 4		@ drain WB
	mcr	p15, 0, r0, c2, c0, 0		@ load page table pointer
	mcr	p15, 0, ip, c8, c7, 0		@ invalidate I & D TLBs
	mov	pc, lr

DECLARE_CPU_OP(cpu_switch_ttb, arm926_switch_mm)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_flush_kern_cache_all)
	mov	r2, #VM_EXEC
	mov	ip, #0
__flush_whole_cache:
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, ip, c7, c6, 0		@ invalidate D cache
#else
1:	mrc	p15, 0, r15, c7, c14, 3 	@ test,clean,invalidate
	bne	1b
#endif
	tst	r2, #VM_EXEC
	mcrne	p15, 0, ip, c7, c5, 0		@ invalidate I cache
	mcrne	p15, 0, ip, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_cache_all, arm926_flush_kern_cache_all)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_flush_kern_dcache_page)
	add	r1, r0, #PAGE_SIZE
1:	mcr	p15, 0, r0, c7, c14, 1		@ clean+invalidate D entry
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0		@ invalidate I cache
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_cache_page, arm926_flush_kern_dcache_page)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_flush_user_cache_range)
	mov	ip, #0
	sub	r3, r1, r0			@ calculate total size
	cmp	r3, #CACHE_DLIMIT
	bgt	__flush_whole_cache
1:	tst	r2, #VM_EXEC
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
#else
	mcr	p15, 0, r0, c7, c14, 1		@ clean and invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
	mcr	p15, 0, r0, c7, c14, 1		@ clean and invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
#endif
	cmp	r0, r1
	blo	1b
	tst	r2, #VM_EXEC
	mcrne	p15, 0, ip, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_cache_range, arm926_flush_user_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_clean_cache_range)
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
	add	r0, r0, #CACHE_DLINESIZE
	subs	r1, r1, #CACHE_DLINESIZE
	bhi	1b
#endif
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_clean_cache_range, arm926_clean_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(arm926_flush_dcache_entry)
        mcr     p15, 0, r0, c7, c10, 1
        mov     r0, #0
        mcr     p15, 0, r0, c7, c10, 4
        mov     pc, lr

DECLARE_CPU_OP(cpu_flush_cache_entry, arm926_flush_dcache_entry)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(arm926_dma_inv_range)
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
	tst	r0, #CACHE_DLINESIZE - 1
	mcrne	p15, 0, r0, c7, c10, 1		@ clean D entry
	tst	r1, #CACHE_DLINESIZE - 1
	mcrne	p15, 0, r1, c7, c10, 1		@ clean D entry
#endif
	bic	r0, r0, #CACHE_DLINESIZE - 1
1:	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D entry
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_invalidate_dma_range, arm926_dma_inv_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(arm926_dma_clean_range)
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
	bic	r0, r0, #CACHE_DLINESIZE - 1
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
#endif
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_clean_dma_range, arm926_dma_clean_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(arm926_dma_flush_range)
	bic	r0, r0, #CACHE_DLINESIZE - 1
1:
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, r0, c7, c14, 1		@ clean+invalidate D entry
#else
	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
#endif
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_dma_range, arm926_dma_flush_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(arm926_coherent_kern_range)
	/* FALLTHROUGH */

ENTRY(arm926_coherent_user_range)
	bic	r0, r0, #CACHE_DLINESIZE - 1
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
	mcr	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

DECLARE_CPU_OP(cpu_coherent_range, arm926_coherent_user_range)
