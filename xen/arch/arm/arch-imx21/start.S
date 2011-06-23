/*
 * start.S 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          ChanJu Park  <bestworld@samsung.com>
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
#include <xen/config.h>
#include <asm/cpu-domain.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/memmap.h>

        .type   text, %function
ENTRY(text)
        msr     cpsr_c, #PSR_F_BIT | PSR_I_BIT | PSR_MODE_SVC

        adr     r1, __SRA_INFO
        str     r0, [r1, #0]

        mcr    p15, 0, ip, c7, c7, 0   @ Invalidate I, D caches & BTB
        mcr    p15, 0, ip, c7, c10, 4  @ Drain Write Buffer
        mcr    p15, 0, ip, c8, c7, 0   @ Invalidate I, D TLBs

	/* clear page table area */
pgd_setup:
	adr     r0, text
	mov	r1, r0
	sub     r0, r0, #0x4000
	mov     r2, #0
1:      str     r2, [r1, #-4]!
	str     r2, [r1, #-4]!
	str     r2, [r1, #-4]!
	str     r2, [r1, #-4]!
	cmp     r0, r1
	bne     1b

	adr	r4, mem_map_table
	b	2f

1:
	str	r3, [r0, r2]
	add	r2, r2, #4
	add	r3, r3, #0x100000
	adds	r1, r1, #-1
	bhi	1b
2:
	ldmia	r4!, {r1, r2, r3}
	cmp	r1, #0
	bne	1b

dac_setup:
	mov     r5, #(DOMAIN_VALUE(DOMAIN_SUPERVISOR, DOMAIN_MANAGER)	| \
		      DOMAIN_VALUE(DOMAIN_HYPERVISOR, DOMAIN_MANAGER)	| \
		      DOMAIN_VALUE(DOMAIN_USER, DOMAIN_MANAGER)		| \
		      DOMAIN_VALUE(DOMAIN_IO, DOMAIN_CLIENT))

	mcr     p15, 0, r5, c3, c0, 0	@ Load DAC

	mcr     p15, 0, r0, c2, c0, 0	@ Load page table pointer
	mrc     p15, 0, r6, c1, c0, 0

	orr     r6, r6, #CR_M           @ Enable Memory Management Unit
	orr     r6, r6, #CR_C           @ Enable Data Cache
	orr     r6, r6, #CR_W           @ Enable Write Buffer
	orr     r6, r6, #CR_I           @ Enable Instruction Cache
	orr     r6, r6, #CR_V           @ Use High Vector Table

	mcr     p15, 0, r6, c1, c0, 0   @ Turn on MMU
	mrc     p15, 0, r3, c0, c0, 0
	mov     r0, r0
	mov     r0, r0
	mov     r0, r0

        mcr	p15, 0, ip, c7, c7, 0	@ Invalidate I, D caches & BTB
        mcr	p15, 0, ip, c7, c10, 4	@ Drain Write (& Fill) Buffer
        mcr	p15, 0, ip, c8, c7, 0	@ Invalidate I, D TLBs

	b	clear_bss

1:	.word   _sbss
	.word   _ebss

clear_bss:
	adr     r0, 1b
	ldmia   r0, {r1, r2}
	mov     r0, #0
1:
	str     r0, [r1], #4
	cmp     r1, r2
	blo     1b

stack_setup:
	msr	cpsr_c, #PSR_MODE_IRQ | PSR_I_BIT | PSR_F_BIT
	ldr	sp, =(irq_stack + STACK_SIZE)

	msr	cpsr_c, #PSR_MODE_ABT | PSR_I_BIT | PSR_F_BIT
	ldr	sp, =(abt_stack + STACK_SIZE)

	msr	cpsr_c, #PSR_MODE_UND | PSR_I_BIT | PSR_F_BIT
	ldr	sp, =(und_stack + STACK_SIZE)

	msr	cpsr_c, #PSR_MODE_SVC | PSR_I_BIT | PSR_F_BIT
	ldr	sp, =(svc_stack + STACK_SIZE)

	adr     r12, 1f
	ldmia   r12, {lr, pc}

1:
	.long   0
	.long   start_xen

ENTRY(arch_context_switch)
	mov	pc, lr

mem_map_table:
	MAP_ENTRY(0xC0000000, 0xC0000000, 64, PDE_TYPE_HYPERVISOR)
	MAP_ENTRY(0xFF000000, 0xC0000000, 2, PDE_TYPE_HYPERVISOR)
	MAP_ENTRY(0xE0000000, 0x10000000, 1, PDE_TYPE_IO)
	MAP_ENTRY(0,0,0,0)

ENTRY(sra_info_ptr)
__SRA_INFO:
	.long	0

	.section .bss.stack_aligned,"w"
ENTRY(svc_stack)
	.fill STACK_SIZE,1,0
ENTRY(irq_stack)
	.fill STACK_SIZE,1,0
ENTRY(abt_stack)
	.fill STACK_SIZE,1,0
ENTRY(und_stack)
	.fill STACK_SIZE,1,0
