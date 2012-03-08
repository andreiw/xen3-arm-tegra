/*
 *  xen/arch/arm/mach-tegra/gic.c
 *
 *  Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *  Copyright (C) 2002 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Interrupt architecture for the GIC:
 *
 * o There is one Interrupt Distributor, which receives interrupts
 *   from system devices and sends them to the Interrupt Controllers.
 *
 * o There is one CPU Interface per CPU, which sends interrupts sent
 *   by the Distributor, and interrupts generated locally, to the
 *   associated CPU. The base address of the CPU interface is usually
 *   aliased so that the same address points to different chips depending
 *   on the CPU it is accessed from.
 *
 * Note that IRQs 0-31 are special - they are local to each CPU.
 * As such, the enable set/clear, pending set/clear and active bit
 * registers are banked per-cpu for these sources.
 *
 */

#include <asm/io.h>
#include <xen/init.h>
#include <xen/types.h>
#include <asm/irq.h>
#include <xen/smp.h>
#include <xen/cpumask.h>
#include <xen/spinlock.h>
#include <xen/errno.h>
#include <asm/gic.h>

/*
 * Supported arch specific GIC irq extension.
 * Default make them NULL.
 */
struct irqchip gic_arch_extn = {
	.ack		= NULL,
	.mask		= NULL,
	.unmask		= NULL,
	.retrigger	= NULL,
	.set_type	= NULL,
	.wake		= NULL,
};

static DEFINE_SPINLOCK(irq_controller_lock);

struct gic_chip_data {
	unsigned int irq_offset;
	void *dist_base;
	void *cpu_base;
	unsigned int max_irq;
};

#ifndef MAX_GIC_NR
#define MAX_GIC_NR	1
#endif

static struct gic_chip_data gic_data[MAX_GIC_NR];

static inline void *gic_dist_base(unsigned int irq)
{
	struct gic_chip_data *gic_data = get_irq_chip_data(irq);
	return gic_data->dist_base;
}

static inline void  *gic_cpu_base(unsigned int irq)
{
	struct gic_chip_data *gic_data = get_irq_chip_data(irq);
	return gic_data->cpu_base;
}

static inline unsigned int gic_irq(unsigned int irq)
{
	struct gic_chip_data *gic_data = get_irq_chip_data(irq);
	return irq - gic_data->irq_offset;
}

/*
 * Routines to acknowledge, disable and enable interrupts
 *
 * Linux assumes that when we're done with an interrupt we need to
 * unmask it, in the same way we need to unmask an interrupt when
 * we first enable it.
 *
 * The GIC has a separate notion of "end of interrupt" to re-enable
 * an interrupt after handling, in order to support hardware
 * prioritisation.
 *
 * We can make the GIC behave in the way that Linux expects by making
 * our "acknowledge" routine disable the interrupt, then mark it as
 * complete.
 */
static void gic_ack_irq(unsigned int irq)
{
	u32 mask = 1 << (irq % 32);

	spin_lock(&irq_controller_lock);
	if (gic_arch_extn.ack)
		gic_arch_extn.ack(irq);
	writel(mask, gic_dist_base(irq) + GIC_DIST_ENABLE_CLEAR + (gic_irq(irq) / 32) * 4);
	writel(gic_irq(irq), gic_cpu_base(irq) + GIC_CPU_EOI);
	spin_unlock(&irq_controller_lock);
}

static void gic_mask_irq(unsigned int irq)
{
	u32 mask = 1 << (irq % 32);

	spin_lock(&irq_controller_lock);
	writel(mask, gic_dist_base(irq) + GIC_DIST_ENABLE_CLEAR + (gic_irq(irq) / 32) * 4);
	if (gic_arch_extn.mask)
		gic_arch_extn.mask(irq);
	spin_unlock(&irq_controller_lock);
}

static void gic_unmask_irq(unsigned int irq)
{
	u32 mask = 1 << (irq % 32);

	spin_lock(&irq_controller_lock);
	if (gic_arch_extn.unmask)
		gic_arch_extn.unmask(irq);
	writel(mask, gic_dist_base(irq) + GIC_DIST_ENABLE_SET + (gic_irq(irq) / 32) * 4);
	spin_unlock(&irq_controller_lock);
}

static int gic_retrigger(unsigned int irq)
{
	if (gic_arch_extn.retrigger)
		return gic_arch_extn.retrigger(irq);

	return -ENXIO;
}

static int gic_set_type(unsigned int irq, unsigned int type)
{
	void *base = gic_dist_base(irq);
	unsigned int gicirq = gic_irq(irq);
	u32 enablemask = 1 << (gicirq % 32);
	u32 enableoff = (gicirq / 32) * 4;
	u32 confmask = 0x2 << ((gicirq % 16) * 2);
	u32 confoff = (gicirq / 16) * 4;
	bool enabled = false;
	u32 val;

	/* Interrupt configuration for SGIs can't be changed */
	if (gicirq < 16)
		return -EINVAL;

	if (type != IRQ_TYPE_LEVEL_HIGH && type != IRQ_TYPE_EDGE_RISING)
		return -EINVAL;

	spin_lock(&irq_controller_lock);

	if (gic_arch_extn.set_type)
		gic_arch_extn.set_type(irq, type);

	val = readl(base + GIC_DIST_CONFIG + confoff);
	if (type == IRQ_TYPE_LEVEL_HIGH)
		val &= ~confmask;
	else if (type == IRQ_TYPE_EDGE_RISING)
		val |= confmask;

	/*
	 * As recommended by the spec, disable the interrupt before changing
	 * the configuration
	 */
	if (readl(base + GIC_DIST_ENABLE_SET + enableoff) & enablemask) {
		writel(enablemask, base + GIC_DIST_ENABLE_CLEAR + enableoff);
		enabled = true;
	}

	writel(val, base + GIC_DIST_CONFIG + confoff);

	if (enabled)
		writel(enablemask, base + GIC_DIST_ENABLE_SET + enableoff);

	spin_unlock(&irq_controller_lock);
	return 0;
}

#ifdef CONFIG_SMP
int gic_set_cpu(unsigned int irq, const cpumask_t *mask_val)
{
	void *reg = gic_dist_base(irq) + GIC_DIST_TARGET + (gic_irq(irq) & ~3);
	unsigned int shift = (irq % 4) * 8;
	unsigned int cpu = cpumask_first(mask_val);
	u32 val;

	spin_lock(&irq_controller_lock);
	irqdesc[irq].node = cpu;
	val = readl(reg) & ~(0xff << shift);
	val |= 1 << (cpu + shift);
	writel(val, reg);
	spin_unlock(&irq_controller_lock);

	return 0;
}
#endif

static void gic_handle_cascade_irq(unsigned int irq, struct irqdesc *desc,
	struct cpu_user_regs *regs)
{
	struct gic_chip_data *chip_data = get_irq_data(irq);
	struct irqchip *chip = get_irq_chip(irq);
	unsigned int cascade_irq, gic_irq;
	unsigned long status;

	/* primary controller ack'ing */
	chip->ack(irq);

	spin_lock(&irq_controller_lock);
	status = readl(chip_data->cpu_base + GIC_CPU_INTACK);
	spin_unlock(&irq_controller_lock);

	gic_irq = (status & 0x3ff);
	if (gic_irq == 1023)
		goto out;

	cascade_irq = gic_irq + chip_data->irq_offset;
	if (unlikely(gic_irq < 32 || gic_irq > 1020 || cascade_irq >= NR_IRQS))
		do_bad_IRQ(cascade_irq, desc);
	else
		desc->handle(cascade_irq, desc, regs);

out:
	/* primary controller unmasking */
	chip->unmask(irq);
}

static int gic_wake(unsigned int irq, unsigned int on)
{
	if (gic_arch_extn.wake)
		return gic_arch_extn.wake(irq, on);

	return -ENXIO;
}

static struct irqchip gic_chip = {
	.name		= "GIC",
	.ack		= gic_ack_irq,
	.mask		= gic_mask_irq,
	.unmask		= gic_unmask_irq,
	.set_type	= gic_set_type,
#ifdef CONFIG_SMP
	.set_affinity	= gic_set_cpu,
#endif
	.wake		= gic_wake,
};

static unsigned int _gic_dist_init(unsigned int gic_nr)
{
	unsigned int max_irq, i;
	void *base = gic_data[gic_nr].dist_base;
	u32 cpumask = 1 << smp_processor_id();

	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	writel(0, base + GIC_DIST_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	max_irq = readl(base + GIC_DIST_CTR) & 0x1f;
	max_irq = (max_irq + 1) * 32;

	/*
	 * The GIC only supports up to 1020 interrupt sources.
	 * Limit this to either the architected maximum, or the
	 * platform maximum.
	 */
	if (max_irq > max(1020, NR_IRQS))
		max_irq = max(1020, NR_IRQS);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = 32; i < max_irq; i += 16)
		writel(0, base + GIC_DIST_CONFIG + i * 4 / 16);

	/*
	 * Set all global interrupts to this CPU only.
	 */
	for (i = 32; i < max_irq; i += 4)
		writel(cpumask, base + GIC_DIST_TARGET + i * 4 / 4);

	/*
	 * Set priority on all interrupts.
	 */
	for (i = 0; i < max_irq; i += 4)
		writel(0xa0a0a0a0, base + GIC_DIST_PRI + i * 4 / 4);

	/*
	 * Disable all interrupts.
	 */
	for (i = 0; i < max_irq; i += 32)
		writel(0xffffffff, base + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);

	return max_irq;
}

static void _gic_dist_exit(unsigned int gic_nr)
{
	writel(0, gic_data[gic_nr].dist_base + GIC_DIST_CTRL);
}

#ifdef CONFIG_PM
void gic_dist_save(unsigned int gic_nr)
{
	unsigned int max_irq = gic_data[gic_nr].max_irq;
	void *dist_base = gic_data[gic_nr].dist_base;
	int i;

	if (gic_nr >= MAX_GIC_NR)
		BUG();

	_gic_dist_exit(gic_nr);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 16); i++)
		gic_data[gic_nr].saved_conf[i] =
			readl(dist_base + GIC_DIST_CONFIG + i * 4);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 4); i++)
		gic_data[gic_nr].saved_pri[i] =
			readl(dist_base + GIC_DIST_PRI + i * 4);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 4); i++)
		gic_data[gic_nr].saved_target[i] =
			readl(dist_base + GIC_DIST_TARGET + i * 4);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 32); i++)
		gic_data[gic_nr].saved_enable[i] =
			readl(dist_base + GIC_DIST_ENABLE_SET + i * 4);
}

void gic_dist_restore(unsigned int gic_nr)
{
	unsigned int max_irq;
	unsigned int i;
	void *dist_base;
	void *cpu_base;

	if (gic_nr >= MAX_GIC_NR)
		BUG();

	_gic_dist_init(gic_nr);

	max_irq = gic_data[gic_nr].max_irq;
	dist_base = gic_data[gic_nr].dist_base;
	cpu_base = gic_data[gic_nr].cpu_base;

	for (i = 0; i < DIV_ROUND_UP(max_irq, 16); i++)
		writel(gic_data[gic_nr].saved_conf[i],
			dist_base + GIC_DIST_CONFIG + i * 4);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 4); i++)
		writel(gic_data[gic_nr].saved_pri[i],
			dist_base + GIC_DIST_PRI + i * 4);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 4); i++)
		writel(gic_data[gic_nr].saved_target[i],
			dist_base + GIC_DIST_TARGET + i * 4);

	for (i = 0; i < DIV_ROUND_UP(max_irq, 32); i++)
		writel(gic_data[gic_nr].saved_enable[i],
			dist_base + GIC_DIST_ENABLE_SET + i * 4);

	writel(1, dist_base + GIC_DIST_CTRL);
	writel(0xf0, cpu_base + GIC_CPU_PRIMASK);
	writel(1, cpu_base + GIC_CPU_CTRL);
}
#endif

void __init gic_dist_init(unsigned int gic_nr, void *base,
			  unsigned int irq_start)
{
	unsigned int max_irq;
	unsigned int i;

	if (gic_nr >= MAX_GIC_NR)
		BUG();

	gic_data[gic_nr].dist_base = base;
	gic_data[gic_nr].irq_offset = (irq_start - 1) & ~31;

	max_irq = _gic_dist_init(gic_nr);
	gic_data[gic_nr].max_irq = max_irq;

	/*
	 * Setup the Linux IRQ subsystem.
	 */
	for (i = irq_start; i < gic_data[gic_nr].irq_offset + max_irq; i++) {
		set_irq_chip(i, &gic_chip);
		set_irq_chip_data(i, &gic_data[gic_nr]);
		set_irq_handler(i, level_irq_handler);
		set_irq_flags(i, IRQF_VALID);
	}

	writel(1, base + GIC_DIST_CTRL);
}

void gic_dist_exit(unsigned int gic_nr)
{
	if (gic_nr >= MAX_GIC_NR)
		BUG();

	_gic_dist_exit(gic_nr);
}

void __init gic_cpu_init(unsigned int gic_nr, void *base)
{
	if (gic_nr >= MAX_GIC_NR)
		BUG();

	gic_data[gic_nr].cpu_base = base;
	writel(0xf0, base + GIC_CPU_PRIMASK);
	writel(1, base + GIC_CPU_CTRL);
}

void gic_cpu_exit(unsigned int gic_nr)
{
	if (gic_nr >= MAX_GIC_NR)
		BUG();

	writel(0, gic_data[gic_nr].cpu_base + GIC_CPU_CTRL);
}

#ifdef CONFIG_SMP
void gic_raise_softirq(const cpumask_t *mask, unsigned int irq)
{
	unsigned long map = *cpus_addr(*mask);

	/* this always happens on GIC0 */
	writel(map << 16 | irq, gic_data[0].dist_base + GIC_DIST_SOFTINT);
}
#endif