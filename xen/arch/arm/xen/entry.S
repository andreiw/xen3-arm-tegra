/*
 * entry.S
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of
 * Xen on ARM and the associated access control.
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
#include <asm/init.h>
#include <asm/linkage.h>
#include <asm/arch/irqs.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/asm-macros.h>
#include <asm/cpu-domain.h>
#include <asm/asm-offsets.h>
#include <asm/profile.h>
#include <asm/arch/entry-macro.S>

	.macro SAVE_CONTEXT	 offset correction
		sub     lr, lr, #\correction
		str     r0, [sp, #-16]
		str     lr, [sp, #-12]

		mrs     r0, spsr
		mov     lr, #\offset
		str     r0, [sp, #-8]
		str     lr, [sp, #-4]

		sub     r0, sp, #16

		msr     cpsr_cxsf, #(PSR_I_BIT | PSR_F_BIT | PSR_MODE_SVC)

		sub		sp, sp, #CTXT_FRAME_SIZE
SPFIX(	tst		sp, #4		)
SPFIX(	bicne	sp, sp, #4	)
		stmib	sp, {r1 - lr}^
		ldmia	r0, {r1 - r4}
		add		r5, sp, #CTXT_SSP
		add		r0, sp, #CTXT_FRAME_SIZE
SPFIX(	addne	r0, r0, #4	)
		str		r1, [sp]
		mov		r1, lr
		stmia	r5, {r0 - r4}
		msr		spsr_cxsf, r3
	.endm

	.macro RESTORE_CONTEXT
		msr		cpsr_c, #(PSR_I_BIT | PSR_F_BIT | PSR_MODE_SVC)
		ldr		r0, [sp, #CTXT_SPSR]
		msr		spsr_cxsf, r0
		ldmia	sp, {r0 - lr}^
		add		sp, sp, #CTXT_SSP
		ldmia	sp, {sp, lr, pc}^
	.endm

		.align	5
		.global exception_vectors_table
exception_vectors_table:
		ldr		pc, .rst
		ldr		pc, .und
		ldr		pc, .swi
		ldr		pc, .pabt
		ldr		pc, .dabt
		ldr		pc, .adx
		ldr		pc, .irq
		ldr		pc, .fiq

.rst:	.long	vector_reset
.und:	.long	vector_undefined_instruction
.swi:	.long	vector_swi
.pabt:	.long	vector_prefetch_abort
.dabt:	.long	vector_data_abort
.adx:	.long	vector_addrexcptn
.irq:	.long	vector_irq
.fiq:	.long	vector_fiq

		.align	5
vector_reset:
1:
		b	1b

		.align	5
vector_irq:
		SAVE_CONTEXT	0x18, 4

		get_irqnr_preamble r5, lr
1:
		get_irqnr_and_base r0, r6, r5, lr
		movne   r1, sp

		adrne   lr, 1b
		bne		asm_do_IRQ

		mrs		r0, spsr
		and		r0, r0, #PSR_MODE_MASK
		eors	r0, r0, #PSR_MODE_SVC

		bne		return_to_guest
	
		RESTORE_CONTEXT

		.align	5
vector_data_abort:
		str		r0,	[sp, #-16]
		str		lr,	[sp, #-12]
		mrs     r0, spsr
		str     r0, [sp, #-8]
		sub     r0, sp, #16

		msr     cpsr_cxsf, #(PSR_I_BIT | PSR_F_BIT | PSR_MODE_SVC)

		sub     sp, sp, #CTXT_FRAME_SIZE
SPFIX(  tst     sp, #4      )
SPFIX(  bicne   sp, sp, #4  )
		stmib   sp, {r1 - lr}^
		ldmia   r0, {r1 - r3}
		add     r5, sp, #CTXT_SSP
		add     r0, sp, #CTXT_FRAME_SIZE
SPFIX(  addne   r0, r0, #4  )
		str     r1, [sp]
		mov     r1, lr
		stmia   r5, {r0 - r3}

		mrc     p15, 0, r0, c6, c0, 0
		mrc     p15, 0, r1, c5, c0, 0

		and		r4, r3, #PSR_MODE_MASK
		eors	r4, r4, #PSR_MODE_SVC

		beq		do_data_abort

		ENABLE_INTERRUPTS
		
		cci     r8
		ldr     r9, [r8]

		ldr     r10, [r9, #OFFSET_VCPU_INFO ]
		ldr     r14, [r9, #(OFFSET_ARCH_VCPU + OFFSET_TRAP_TABLE + OFFSET_VECTOR_DABT)]
		cmp     r14, #0
		beq     trap_table_invalid

		str     r0, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_VFAR)]
		str     r1, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_VFSR)]

		mov     r9, #0x01
		strb    r9, [r10, #OFFSET_EVTCHN_UPCALL_MASK]

		ldr     r0, [sp, #CTXT_USP]
		ldr     r1, [sp, #CTXT_ULR]

		ldr     r5, [r8, #OFFSET_VPSR]
		bic     r3, r3, #PSR_MODE_MASK
		orr     r3, r3, r5

		str     r0, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSP)]
		str     r1, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TLR)]
		str     r3, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSPSR)]

		cmp     r5, #PSR_MODE_SVC
		ldrne   r0, [r8, #8]

		mov     r5, #PSR_MODE_SVC
		str     r5, [r8, #4]
		str     r0, [r8, #8]
		str     r2, [r8, #12]

		ldr     r5, =DOMAIN_KERNEL_VALUE
		mcr     p15, 0, r5, c3, c0, 0

		DISABLE_INTERRUPTS

		add     r8, r8, #8
		ldmia   r8, {r13, r14}^
		ldmia   sp, {r0-r12}
		ldr     sp, [sp, #CTXT_SSP]
		msr     spsr, #PSR_MODE_USR
		movs    pc, lr

		.align	5
vector_prefetch_abort:
		str     r0, [sp, #-16]
		str     lr, [sp, #-12]
		mrs     r0, spsr
		str     r0, [sp, #-8]
		sub     r0, sp, #16

		msr     cpsr_cxsf, #(PSR_I_BIT | PSR_F_BIT | PSR_MODE_SVC)

		sub     sp, sp, #CTXT_FRAME_SIZE
SPFIX(  tst     sp, #4      )
SPFIX(  bicne   sp, sp, #4  )
		stmib   sp, {r1 - lr}^
		ldmia   r0, {r1 - r3}
		add     r5, sp, #CTXT_SSP
		add     r0, sp, #CTXT_FRAME_SIZE
SPFIX(  addne   r0, r0, #4  )
		str     r1, [sp]
		mov     r1, lr
		stmia   r5, {r0 - r3}

		mrc     p15, 0, r0, c6, c0, 0
		mrc     p15, 0, r1, c5, c0, 0

		and     r4, r3, #PSR_MODE_MASK
		eors    r4, r4, #PSR_MODE_SVC

		beq		do_prefetch_abort

		ENABLE_INTERRUPTS

		cci     r8
		ldr     r9, [r8]

		ldr     r10, [r9, #OFFSET_VCPU_INFO]
		ldr     r14, [r9, #(OFFSET_ARCH_VCPU + OFFSET_TRAP_TABLE + OFFSET_VECTOR_PABT)]
		cmp     lr, #0
		beq     trap_table_invalid

		mov     r9, #0x01
		strb    r9, [r10, #OFFSET_EVTCHN_UPCALL_MASK]

		ldr     r0, [sp, #CTXT_USP]
		ldr     r1, [sp, #CTXT_ULR]

		ldr     r5, [r8, #4]
		bic     r3, r3, #PSR_MODE_MASK
		orr     r3, r3, r5

		str     r0, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSP)]
		str     r1, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TLR)]
		str     r3, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSPSR)]

		cmp     r5, #PSR_MODE_SVC
		ldrne   r0, [r8, #8]

		mov     r5, #PSR_MODE_SVC
		str     r5, [r8, #4]
		str     r0, [r8, #8]
		str     r2, [r8, #12]

		ldr     r5, =DOMAIN_KERNEL_VALUE
		mcr     p15, 0, r5, c3, c0, 0

		DISABLE_INTERRUPTS

		add     r8, r8, #8
		ldmia   r8, {r13, r14}^
		ldmia   sp, {r0-r12}
		ldr     sp, [sp, #CTXT_SSP]
		msr     spsr, #PSR_MODE_USR
		movs    pc, lr

		.align	5
vector_undefined_instruction:
		str     r0, [sp, #-16]
		str     lr, [sp, #-12]
		mrs     r0, spsr
		str     r0, [sp, #-8]
		sub     r0, sp, #16

		msr     cpsr_cxsf, #(PSR_I_BIT | PSR_F_BIT | PSR_MODE_SVC)

		sub     sp, sp, #CTXT_FRAME_SIZE
SPFIX(  tst     sp, #4      )
SPFIX(  bicne   sp, sp, #4  )
		stmib   sp, {r1 - lr}^
		ldmia   r0, {r1 - r3}
		add     r5, sp, #CTXT_SSP
		add     r0, sp, #CTXT_FRAME_SIZE
SPFIX(  addne   r0, r0, #4  )
		str     r1, [sp]
		mov     r1, lr
		stmia   r5, {r0 - r3}

		and     r4, r3, #PSR_MODE_MASK
		eors    r4, r4, #PSR_MODE_SVC

		beq     do_undefined_instruction

		ENABLE_INTERRUPTS

		cci     r8
		ldr     r9, [r8]

		ldr     r10, [r9, #OFFSET_VCPU_INFO]
		ldr     r14, [r9, #(OFFSET_ARCH_VCPU + OFFSET_TRAP_TABLE + OFFSET_VECTOR_UND)]
		cmp     lr, #0
		beq     trap_table_invalid

		mov     r9, #0x01
		strb    r9, [r10, #OFFSET_EVTCHN_UPCALL_MASK]

		ldr     r0, [sp, #CTXT_USP]
		ldr     r1, [sp, #CTXT_ULR]

		ldr     r5, [r8, #4]
		bic     r3, r3, #PSR_MODE_MASK
		orr     r3, r3, r5

		str     r0, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSP)]
		str     r1, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TLR)]
		str     r3, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSPSR)]

		cmp     r5, #PSR_MODE_SVC
		ldrne   r0, [r8, #8]

		mov     r5, #PSR_MODE_SVC
		str     r5, [r8, #4]
		str     r0, [r8, #8]
		str     r2, [r8, #12]
		str     r1, [r8, #16]

		ldr     r5, =DOMAIN_KERNEL_VALUE
		mcr     p15, 0, r5, c3, c0, 0

		DISABLE_INTERRUPTS

		add     r8, r8, #8
		ldmia   r8, {r13, r14}^
		ldmia   sp, {r0-r12}
		ldr     sp, [sp, #CTXT_SSP]
		msr     spsr, #PSR_MODE_USR
		movs    pc, lr

		.align	5
vector_fiq:
		disable_fiq
		subs    pc, lr, #4

		.align	5
vector_addrexcptn:
		b	vector_addrexcptn

		.align	5
trap_table_invalid:
		b	trap_table_invalid

		.align	5
vector_swi:
		str		sp, [sp, #(CTXT_SSP - CTXT_FRAME_SIZE)]
		sub     sp, sp, #CTXT_FRAME_SIZE
		stmia   sp, {r0 - lr}^
		mrs     r11, spsr
		str     r14, [sp, #CTXT_PC]
		str     r11, [sp, #CTXT_SPSR]

		ENABLE_INTERRUPTS

		cci		r8
		ldr		r12, [r8, #4]
		eors	r12, #PSR_MODE_SVC

		beq		invoke_hypercall
		
		mov		r12, #PSR_MODE_SVC
		str     r12, [r8, #4]
		str     r14, [r8, #12]

		ldr     r9, [r8]
		ldr     r10, [r9, #OFFSET_VCPU_INFO]
		ldr     r14, [r9, #(OFFSET_ARCH_VCPU + OFFSET_TRAP_TABLE + OFFSET_VECTOR_SWI)]
		cmp     r14, #0
		beq		trap_table_invalid

		mov     r9, #0x01
		strb    r9, [r10, #OFFSET_EVTCHN_UPCALL_MASK]

		ldr     r4, [sp, #CTXT_USP]
		ldr     r5, [sp, #CTXT_ULR]

		str     r4, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSP)]
		str     r5, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TLR)]
		str     r11, [r10, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSPSR)]

		ldr     r11, =DOMAIN_KERNEL_VALUE
		mcr     p15, 0, r11, c3, c0, 0

		DISABLE_INTERRUPTS

		add     r8, r8, #8
		ldmia   r8, {r13, r14}^
		ldmia   sp, {r0-r12}
		ldr     sp, [sp, #CTXT_SSP]
		msr     spsr, #PSR_MODE_USR
		movs    pc, lr

invoke_hypercall:
		ldr     r12, [lr, #-4]
		bic     r12, r12, #0xff000000

		adr     r14, 1f
		adr     r11, hypercall_table
		ldr     pc, [r11, r12, lsl #2]

1:
		str     r0, [sp, #CTXT_R0]

		b		return_to_guest

ENTRY(return_to_guest)	
		ENABLE_INTERRUPTS

		ldr		r4, =irq_stat
		ldr		r5, [r4, #OFFSET_SOFTIRQ_PENDING]
		teq		r5, #0

		blne	do_softirq

		cci		r8
		ldr		r10, [r8, #OFFSET_VCPU]

		ldr		r11, [r10, #OFFSET_VCPU_INFO]
		ldrb	r9, [r11, #OFFSET_EVTCHN_UPCALL_MASK]
		tst		r9, #0xFF
		bne		resume_guest_domain

		ldrb	r12, [r11, #OFFSET_EVTCHN_UPCALL_PENDING]
		tst		r12, #0xFF
		beq		resume_guest_domain

do_upcall:
		ldr     r14, [r10, #(OFFSET_ARCH_VCPU + OFFSET_TRAP_TABLE + OFFSET_VECTOR_IRQ)]
		cmp		lr, #0
		beq		trap_table_invalid

		mov		r9, #0x01
		strb	r9, [r11, #OFFSET_EVTCHN_UPCALL_MASK]

		ldr		r0, [sp, #CTXT_USP]
		ldr		r1, [sp, #CTXT_ULR]
		ldr		r2, [sp, #CTXT_PC]
		ldr		r3, [sp, #CTXT_SPSR]

		ldr		r5, [r8, #4]
		bic		r3, r3, #PSR_MODE_MASK
		orr		r3, r3, r5

		str		r0, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSP)]
		str		r1, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TLR)]
		str		r3, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSPSR)]

		cmp		r5, #PSR_MODE_SVC
		ldrne	r0, [r8, #8]

		mov		r5, #PSR_MODE_SVC
		str		r5, [r8, #4]
		str		r0, [r8, #8]
		str		r2, [r8, #12]

		ldr		r5, =DOMAIN_KERNEL_VALUE
		mcr     p15, 0, r5, c3, c0, 0
	
		DISABLE_INTERRUPTS

		add		r8, r8, #8
		ldmia	r8, {r13, r14}^
		ldmia	sp, {r0-r12}
		ldr		sp, [sp, #CTXT_SSP]
		msr		spsr, #PSR_MODE_USR
		movs	pc, lr

resume_guest_domain:
		cci		r8
		ldr		r3, [r8, #OFFSET_VPSR]
		ldr		ip, [sp, #CTXT_SPSR]
		cmp		r3, #PSR_MODE_SVC

		ldrne	r7, =DOMAIN_HYPERVISOR_VALUE
		ldreq   r7, =DOMAIN_KERNEL_VALUE
		mcr		p15, 0, r7, c3, c0, 0

		RESTORE_CONTEXT

/*
 * Prototype : __switch_to(struct vcpu *, struct vcpu_guest_context *, struct vcpu_guest_context *)
 */
		.align	5
ENTRY(__switch_to)
		add     ip, r1, #(OFFSET_USER_REGS + OFFSET_R4)
		stmia   ip, {r4 - sl, fp, ip, sp, lr}

		mrc		p15, 0, r4, c3, c0, 0
		mrc		p15, 0, r5, c1, c0, 0
		mrc		p15, 0, r6, c13, c0, 0

		str		r4, [r1, #(OFFSET_SYS_REGS + OFFSET_DACR)]
		str		r5, [r1, #(OFFSET_SYS_REGS + OFFSET_CR)]
		str		r6, [r1, #(OFFSET_SYS_REGS + OFFSET_PIDR)]

		bl		arch_context_switch

		ldr		r4, [r2, #(OFFSET_SYS_REGS + OFFSET_DACR)]
		ldr		r5, [r2, #(OFFSET_SYS_REGS + OFFSET_CR)]
		ldr		r6, [r2, #(OFFSET_SYS_REGS + OFFSET_PIDR)]

		mcr		p15, 0, r4, c3, c0, 0
		mcr		p15, 0, r5, c1, c0, 0
		mcr		p15, 0, r6, c13, c0, 0

		add		ip, r2, #(OFFSET_USER_REGS + OFFSET_R4)
		ldmia   ip,  {r4 - sl, fp, ip, sp, lr}
 
		mov		pc, lr

		.align	5
		.type hypercall_table, #object
ENTRY(hypercall_table)
		.long	do_set_trap_table						/*  0 */
		.long	do_mmu_update
		.long	do_ni_hypercall
		.long	do_ni_hypercall
		.long	do_set_callbacks
		.long	do_ni_hypercall							/*  5 */
		.long	do_sched_op_compat
		.long	do_dom0_op
		.long	do_ni_hypercall
		.long	do_ni_hypercall
		.long	do_ni_hypercall							/* 10 */
		.long	do_ni_hypercall
		.long	do_memory_op
		.long	do_multicall
		.long	do_update_va_mapping
		.long	do_set_timer_op							/* 15 */
		.long	do_event_channel_op
		.long	do_xen_version
		.long	do_console_io
		.long	do_physdev_op
		.long	do_grant_table_op						/* 20 */
		.long	do_vm_assist
		.long	do_ni_hypercall
		.long	do_restore_trap_frame
		.long	do_vcpu_op
		.long	do_ni_hypercall							/* 25 */
		.long	do_mmuext_op
		.long	do_ni_hypercall
		.long	do_nmi_op
		.long	do_sched_op
		.long	do_set_domain							/* 30 */
		.long	do_ni_hypercall
		.long	do_ni_hypercall
		.long	do_set_foreground_domain
		.long	do_ni_hypercall
		.long	do_ni_hypercall							/* 35 */
		.long	do_ni_hypercall
		.long	do_ni_hypercall
		.long	do_ni_hypercall
#ifdef CONFIG_GCOV_XEN
		.long	do_gcov_op
#else
		.long	do_ni_hypercall
#endif
		.long	do_iwmmxt_op							/* 40 */
		.long	do_ni_hypercall
		.long	do_ni_hypercall
		.long	do_sync_vcpu_state
		.long	do_cache_control
		.long	do_ni_hypercall							/* 45 */
		.long	do_ni_hypercall
		.long	do_ni_hypercall
		.long	fcse_set_pid
		.long	fcse_get_pid
		.long	do_vfp_op								/* 50 */
