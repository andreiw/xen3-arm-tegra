#include <xen/config.h>
#include <asm/io.h>
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
	.trigger_type = "level",
	.ack = tegra_mask_irq,
	.mask = tegra_mask_irq,
	.unmask = tegra_unmask_irq,
};

void tegra_irq_init(void)
{
}

