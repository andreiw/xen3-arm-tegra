#include <asm/io.h>
#include <asm/arch/tegra-regs.h>
#include <xen/init.h>
#include <xen/types.h>
#include <asm/irq.h>

extern struct irqdesc irq_desc[NR_IRQS];
static struct irqchip tegra_internal_chip;
void set_irq_chip(unsigned int irq, struct irqchip *chip);

#define DEBUG_IRQ(fmt...)  do { } while (0)

static void tegra_mask_irq(unsigned int irq)
{
}

static void tegra_unmask_irq(unsigned int irq)
{
}

static struct irqchip tegra_internal_chip = {
	.name = "Tegra",
	.ack = tegra_mask_irq,
	.mask = tegra_mask_irq,
	.unmask = tegra_unmask_irq,
};

void tegra_irq_init(void)
{
        /* gic_dist_init(0, IO_ADDRESS(TEGRA_ARM_INT_DIST_BASE), 29); */
	/* gic_cpu_init(0, IO_ADDRESS(TEGRA_ARM_PERIF_BASE + 0x100)); */
}

