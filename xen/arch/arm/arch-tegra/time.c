#include <xen/sched.h>
#include <xen/irq.h>
#include <xen/init.h>
#include <xen/softirq.h>
#include <xen/spinlock.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/arch/hardware.h>

irqreturn_t tegra_timer_interrupt(int irq, void *dev_id, struct cpu_user_regs *regs)
{
	writel(1<<30, IO_TO_VIRT(TEGRA_TMR3_BASE + TEGRA_TMR_PCR));
	timer_tick(1);
	return IRQ_HANDLED;
}

static struct irqaction tegra_timer_irq = {
	.name		= "Tegra2 Timer Tick",
	.dev_id		= NULL,
	.handler	= tegra_timer_interrupt
};

void tegra_timer_init(void)
{
	unsigned reg;
	unsigned long rate = clk_measure_input_freq();

	switch (rate) {
	case 12000000:
		rate = 0x000b;
		break;
	case 13000000:
		rate = 0x000c;
		break;
	case 19200000:
		rate = 0x045f;
		break;
	case 26000000:
		rate = 0x0019;
		break;
	default:
		printk("System clock rate: %d\n", rate);
		panic("Unknown system clock rate\n");
	}
	writel(rate, IO_TO_VIRT(TEGRA_TMRUS_BASE +
				TEGRA_TMRUS_CFG));

	writel(0, IO_TO_VIRT(TEGRA_TMR3_BASE + TEGRA_TMR_PTV));
	setup_irq(INT_TMR3, &tegra_timer_irq);
	writel(0xC0000000 | ((1000000/HZ)-1),
	       IO_TO_VIRT(TEGRA_TMR3_BASE + TEGRA_TMR_PTV));
}
