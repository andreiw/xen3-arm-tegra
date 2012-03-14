/*
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 * Copyright (C) 2011 Colin Cross <ccross@android.com>
 * Copyright (C) 2011 Google, Inc.
 * Copyright (C) 2010, NVIDIA Corporation
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <asm/io.h>
#include <xen/init.h>
#include <xen/types.h>
#include <xen/bitops.h>
#include <asm/irq.h>
#include <asm/gic.h>
#include <asm/arch/hardware.h>

#define ICTLR_CPU_IEP_VFIQ	0x08
#define ICTLR_CPU_IEP_FIR	0x14
#define ICTLR_CPU_IEP_FIR_SET	0x18
#define ICTLR_CPU_IEP_FIR_CLR	0x1c

#define ICTLR_CPU_IER		0x20
#define ICTLR_CPU_IER_SET	0x24
#define ICTLR_CPU_IER_CLR	0x28
#define ICTLR_CPU_IEP_CLASS	0x2C

#define ICTLR_COP_IER		0x30
#define ICTLR_COP_IER_SET	0x34
#define ICTLR_COP_IER_CLR	0x38
#define ICTLR_COP_IEP_CLASS	0x3c

#define NUM_ICTLRS 4
#define FIRST_LEGACY_IRQ 32

static void __iomem *ictlr_reg_base[] = {
	(void*) IO_TO_VIRT(TEGRA_PRIMARY_ICTLR_BASE),
	(void*) IO_TO_VIRT(TEGRA_SECONDARY_ICTLR_BASE),
	(void*) IO_TO_VIRT(TEGRA_TERTIARY_ICTLR_BASE),
	(void*) IO_TO_VIRT(TEGRA_QUATERNARY_ICTLR_BASE),
};

static inline void tegra_irq_write_mask(unsigned int irq, unsigned long reg)
{
	void __iomem *base;
	u32 mask;

	BUG_ON(irq < FIRST_LEGACY_IRQ ||
		irq >= FIRST_LEGACY_IRQ + NUM_ICTLRS * 32);

	base = ictlr_reg_base[(irq - FIRST_LEGACY_IRQ) / 32];
	mask = BIT((irq - FIRST_LEGACY_IRQ) % 32);

	__raw_writel(mask, base + reg);
}

static void tegra_ack_irq(unsigned int irq)
{
	if (irq < FIRST_LEGACY_IRQ)
		return;

	tegra_irq_write_mask(irq, ICTLR_CPU_IEP_FIR_CLR);
}

static void tegra_mask_irq(unsigned int irq)
{
	if (irq < FIRST_LEGACY_IRQ)
		return;

	tegra_irq_write_mask(irq, ICTLR_CPU_IER_CLR);
}

static void tegra_unmask_irq(unsigned int irq)
{
	if (irq < FIRST_LEGACY_IRQ)
		return;

	tegra_irq_write_mask(irq, ICTLR_CPU_IER_SET);
}

static int tegra_retrigger_irq(unsigned int irq)
{
	if (irq < FIRST_LEGACY_IRQ)
		return 0;

	tegra_irq_write_mask(irq, ICTLR_CPU_IEP_FIR_SET);
	return 0;
}

static struct irqchip tegra_internal_chip = {
	.name = "Tegra",
	.ack = tegra_mask_irq,
	.mask = tegra_mask_irq,
	.unmask = tegra_unmask_irq,
	.retrigger = tegra_retrigger_irq,
};

void tegra_irq_init(void)
{
	int i;

	for (i = 0; i < NUM_ICTLRS; i++) {
		void __iomem *ictlr = ictlr_reg_base[i];
		writel(~0, ictlr + ICTLR_CPU_IER_CLR);
		writel(0, ictlr + ICTLR_CPU_IEP_CLASS);
	}

	gic_arch_extn.ack = tegra_ack_irq;
	gic_arch_extn.mask = tegra_mask_irq;
	gic_arch_extn.unmask = tegra_unmask_irq;
	gic_arch_extn.retrigger = tegra_retrigger_irq;

	gic_dist_init(0, (void *) IO_TO_VIRT(TEGRA_ARM_INT_DIST_BASE), 29);
	gic_cpu_init(0, (void *) IO_TO_VIRT(TEGRA_ARM_PERIF_BASE + 0x100));
}

