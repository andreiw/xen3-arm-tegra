/*
 * core-xscale.S
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

#include <xen/config.h>
#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>
#include <asm/system.h>
#include <asm/asm-macros.h>

#define VM_EXEC		0x00000004

/*
 * the cache line size of the I and D cache
 */

#define CACHE_LINE_SIZE	32
#define MAX_AREA_SIZE	32768

.macro  clean_dcache rd, rs
	mov		\rd, #0x1f00
	orr		\rd, \rd, #0x00e0
1:  mcr		p15, 0, \rd, c7, c14, 2     @ clean/invalidate L1 D line
	adds	\rd, \rd, #0x40000000
	bcc		1b
	subs	\rd, \rd, #0x20
	bpl		1b
.endm

        .text
/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_idle)
	mov		r0, #1
	mcr		p14, 0, r0, c7, c0, 0		@ Go to IDLE
	mov		pc, lr

DECLARE_CPU_OP(cpu_idle, xscale_idle)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_halt)
	stmfd   sp!, {r0, lr}
	mov     r0, #PSR_F_BIT | PSR_I_BIT | PSR_MODE_SVC
	msr     cpsr_c, r0
	bl      xscale_flush_cache_all		@ flush entire caches
	mrc     p15, 0, r0, c1, c0, 0           @
	bic     r0, r0, #0x1800                 @ ...IZ...........
	bic     r0, r0, #0x0006                 @ .............CA.
	mcr     p15, 0, r0, c1, c0, 0           @ disable caches
	cpwait  ip
	ldmfd   sp!, {r0, pc}

DECLARE_CPU_OP(cpu_halt, xscale_halt)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_reset)
	stmfd   sp!, {r1}
	mov     r1, #PSR_F_BIT | PSR_I_BIT | PSR_MODE_SVC
	msr     cpsr_c, r1                      @ reset CPSR
	mcr     p15, 0, r1, c10, c4, 1          @ unlock I-TLB
	mcr     p15, 0, r1, c8, c5, 0           @ invalidate I-TLB
	mrc     p15, 0, r1, c1, c0, 0           @ ctrl register
	bic     r1, r1, #0x0086                 @ ........B....CA.
	bic     r1, r1, #0x3900                 @ ..VIZ..S........
	sub     pc, pc, #4                      @ flush pipeline
	@ *** cache line aligned ***
	mcr     p15, 0, r1, c1, c0, 0           @ ctrl register
	bic     r1, r1, #0x0001                 @ ...............M
	mcr     p15, 0, ip, c7, c7, 0           @ invalidate I,D caches & BTB
	mcr     p15, 0, r1, c1, c0, 0           @ ctrl register
	@ CAUTION: MMU turned off from this point. We count on the pipeline
	@ already containing those two last instructions to survive.
	mcr     p15, 0, ip, c8, c7, 0           @ invalidate I & D TLBs
	cpwait  ip
	ldmfd   sp!, {r1}
	mov     pc, r0

DECLARE_CPU_OP(cpu_reset, xscale_reset)
/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_get_ttb)
	mrc     p15, 0, r0, c2, c0, 0           @ load current page ttb
	mov 	pc, lr

DECLARE_CPU_OP(cpu_get_ttb, xscale_get_ttb)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_switch_ttb)
	cmp		r1, #0
	beq		no_flush
	clean_dcache r3, ip
	mcr 	p15, 0, ip, c7, c5, 0       @ invalidate L1 I cache and BTB
	mcr 	p15, 0, ip, c7, c10, 4      @ data write barrier
	mcr 	p15, 0, ip, c8, c7, 0       @ invalidate I and D TLBs
no_flush:
	mcr 	p15, 0, ip, c7, c5, 4       @ prefetch flush
	mcr 	p15, 0, r0, c2, c0, 0       @ load page table pointer
	cpwait_ret lr, ip

DECLARE_CPU_OP(cpu_switch_ttb, xscale_switch_ttb)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_flush_cache_all)
	mov     r2, #VM_EXEC
	mov     ip, #0
__flush_whole_cache:
	clean_dcache	r0, r1
	tst     r2, #VM_EXEC
	mcrne   p15, 0, ip, c7, c5, 0           @ Invalidate I cache & BTB
	mcrne   p15, 0, ip, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mcrne   p15, 0, ip, c7, c5, 4           @ Prefetch Flush
	cpwait  ip
	mov		pc, lr

DECLARE_CPU_OP(cpu_flush_cache_all, xscale_flush_cache_all)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_flush_cache_page)
	add		r1, r0, #PAGE_SIZE
1:
	mcr		p15, 0, r0, c7, c14, 1			@ Clean / Invalidate D Cache Line
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	mov     r0, #0
	mcr     p15, 0, r0, c7, c5, 0           @ Invalidate I cache & BTB
	mcr     p15, 0, r0, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mcr     p15, 0, r0, c7, c5, 4           @ Prefetch Flush
	cpwait  ip
	mov     pc, lr

DECLARE_CPU_OP(cpu_flush_cache_page, xscale_flush_cache_page)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_flush_cache_range)
	mov		r2, #VM_EXEC
	mov     ip, #0
	sub     r3, r1, r0                      @ calculate total size
	cmp     r3, #MAX_AREA_SIZE
	bhs     __flush_whole_cache

1:
	tst     r2, #VM_EXEC
	mcrne   p15, 0, r0, c7, c5, 1           @ Invalidate I cache line
	mcr		p15, 0, r0, c7, c14, 1			@ Clean/Invalidate D cache line
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	tst		r2, #VM_EXEC
	mcrne   p15, 0, ip, c7, c5, 6           @ Invalidate I cache & BTB
	mcrne   p15, 0, ip, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mcrne   p15, 0, ip, c7, c5, 4           @ Prefetch Flush
	cpwait  ip
	mov     pc, lr

DECLARE_CPU_OP(cpu_flush_cache_range, xscale_flush_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_clean_cache_range)
1:
	mcr		p15, 0, r0, c7, c10, 1          @ clean D entry
	add		r0, r0, #CACHE_LINE_SIZE
	subs	r1, r1, #CACHE_LINE_SIZE
	bhi		1b
	mcr		p15, 0, ip, c7, c10, 4          @ Drain Write (& Fill) Buffer
	cpwait	ip
	mov		pc, lr

DECLARE_CPU_OP(cpu_clean_cache_range, xscale_clean_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(xscale_flush_dcache_entry)
	mcr		p15, 0, r0, c7, c10, 1
	mov		r0, #0
	mcr		p15, 0, r0, c7, c10, 4
	cpwait	ip
	mov		pc, lr

DECLARE_CPU_OP(cpu_flush_cache_entry, xscale_flush_dcache_entry)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(xscale_invalidate_dma_range)
	tst     r0, #CACHE_LINE_SIZE - 1
	bic     r0, r0, #CACHE_LINE_SIZE - 1
	mcrne   p15, 0, r0, c7, c10, 1          @ clean D entry
#ifdef	CONFIG_CPU_MONAHANS_L2CACHE
	mcrne   p15, 1, r0, c7, c11, 1          @ clean L2 D entry
#endif
	tst     r1, #CACHE_LINE_SIZE - 1
	mcrne   p15, 0, r1, c7, c10, 1          @ clean D entry
#ifdef	CONFIG_CPU_MONAHANS_L2CACHE
	mcrne   p15, 1, r0, c7, c11, 1          @ clean L2 D entry
#endif
1:
	mcr     p15, 0, r0, c7, c6, 1           @ invalidate D entry
#ifdef	CONFIG_CPU_MONAHANS_L2CACHE
	mcr     p15, 1, r0, c7, c7, 1           @ invalidate L2 D entry
#endif
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	mcr     p15, 0, r0, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mov     pc, lr
DECLARE_CPU_OP(cpu_invalidate_dma_range, xscale_invalidate_dma_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(xscale_clean_dma_range)
	bic     r0, r0, #CACHE_LINE_SIZE - 1
1:
	mcr     p15, 0, r0, c7, c10, 1          @ clean D entry
#ifdef	CONFIG_CPU_MONAHANS_L2CACHE
	mcr     p15, 1, r0, c7, c11, 1          @ clean L2 D entry
#endif
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	mcr     p15, 0, r0, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mov     pc, lr

DECLARE_CPU_OP(cpu_clean_dma_range, xscale_clean_dma_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(xscale_flush_dma_range)
	bic     r0, r0, #CACHE_LINE_SIZE - 1
1:
	mcr     p15, 0, r0, c7, c10, 1          @ clean D entry
	mcr     p15, 0, r0, c7, c6, 1           @ invalidate D entry
#ifdef	CONFIG_CPU_MONAHANS_L2CACHE
	mcr     p15, 1, r0, c7, c11, 1          @ Clean L2 Cache Line
	mcr     p15, 1, r0, c7, c7, 1           @ Invalidate L2 Cache Line
#endif
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	mcr     p15, 0, r0, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mov     pc, lr
DECLARE_CPU_OP(cpu_flush_dma_range, xscale_flush_dma_range)

ENTRY(xscale_coherent_kern_range)
	bic     r0, r0, #CACHE_LINE_SIZE - 1
1:
	mcr     p15, 0, r0, c7, c10, 1          @ clean D entry
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	mov     r0, #0
	mcr     p15, 0, r0, c7, c5, 0           @ Invalidate I cache & BTB
	mcr     p15, 0, r0, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mov     pc, lr

ENTRY(xscale_coherent_range)
	bic     r0, r0, #CACHE_LINE_SIZE - 1
1:
	mcr     p15, 0, r0, c7, c10, 1          @ clean D entry
	mcr     p15, 0, r0, c7, c5, 1           @ Invalidate I cache entry
	add     r0, r0, #CACHE_LINE_SIZE
	cmp     r0, r1
	blo     1b
	mov     r0, #0
	mcr     p15, 0, r0, c7, c5, 6           @ Invalidate BTB
	mcr     p15, 0, r0, c7, c10, 4          @ Drain Write (& Fill) Buffer
	mcr     p15, 0, r0, c7, c5, 4           @ Prefetch Flush
	cpwait  ip
	mov     pc, lr
DECLARE_CPU_OP(cpu_coherent_range, xscale_coherent_range)

ENTRY(xscale_lock_icache)
	bic		r0, r0, #CACHE_LINE_SIZE - 1
iLockLoop:
	mcr		p15, 0, r0, c9, c1, 0			@ lock into cache
	add		r0, r0, #CACHE_LINE_SIZE		@ advance to next cache line
	cmp		r0, r1							@ are we done?
	blo		iLockLoop
	cpwait  ip
	mov		pc, lr

ENTRY(xscale_unlock_icache)
	bic     r0, r0, #CACHE_LINE_SIZE - 1
iUnLockLoop:
	mcr     p15, 0, r0, c7, c5, 1			@ unlock from cache
	add     r0, r0, #CACHE_LINE_SIZE 		@ advance to next cache line
	cmp     r0, r1							@ are we done?
	blo     iUnLockLoop
	mov     pc, lr

ENTRY(xscale_set_pte)
	str	r1, [r0]
	mov	ip, #0
	mcr     p15, 0, r0, c7, c10, 1          @ Clean D cache line
	mcr     p15, 0, ip, c7, c10, 4          @ Drain Write (& Fill) Buffer
	cpwait	ip
	mov	pc, lr
DECLARE_CPU_OP(cpu_set_pte, xscale_set_pte)

