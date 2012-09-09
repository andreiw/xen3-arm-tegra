/*
 * include/asm-arm/arch-tegra/entry-macro.S
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 * Copyright (C) 2009 Palm, Inc.
 *
 * Low-level IRQ helper macros for Tegra2-based platforms
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <asm/gic.h>
#include <asm/arch/hardware.h>

	/* Uses the GIC interrupt controller built into the cpu */
#define ICTRL_BASE (TEGRA_CPU_VIRT + 0x40100)

	.macro  disable_fiq
	.endm

	.macro	get_irqnr_preamble, base, tmp
	movw \base, #(ICTRL_BASE & 0x0000ffff)
	movt \base, #((ICTRL_BASE & 0xffff0000) >> 16)
	.endm

	/*
	 * The interrupt numbering scheme is defined in the
	 * interrupt controller spec.  To wit:
	 *
	 * Interrupts 0-15 are IPI
	 * 16-28 are reserved
	 * 29-31 are local.  We allow 30 to be used for the watchdog.
	 * 32-1020 are global
	 * 1021-1022 are reserved
	 * 1023 is "spurious" (no interrupt)
	 *
	 * For now, we ignore all local interrupts so only return an interrupt
	 * if it's between 30 and 1020.  The test_for_ipi routine below will
	 * pick up on IPIs.
	 *
	 * A simple read from the controller will tell us the number of the
	 * highest priority enabled interrupt.  We then just need to check
	 * whether it is in the valid range for an IRQ (30-1020 inclusive).
	 */

	.macro  get_irqnr_and_base, irqnr, irqstat, base, tmp

	/* bits 12-10 = src CPU, 9-0 = int # */
	ldr     \irqstat, [\base, #GIC_CPU_INTACK]

	ldr		\tmp, =1021

	bic     \irqnr, \irqstat, #0x1c00

	cmp     \irqnr, #29
	cmpcc	\irqnr, \irqnr
	cmpne	\irqnr, \tmp
	cmpcs	\irqnr, \irqnr

	.endm
