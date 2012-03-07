#include <xen/sched.h>
#include <xen/irq.h>
#include <xen/init.h>
#include <xen/softirq.h>
#include <xen/spinlock.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/arch/hardware.h>

#define TIMERUS_CNTR_1US 0x10
#define TIMERUS_USEC_CFG 0x14
#define TIMERUS_CNTR_FREEZE 0x4c

#define TIMER1_BASE 0x0
#define TIMER2_BASE 0x8
#define TIMER3_BASE 0x50
#define TIMER4_BASE 0x58

#define TIMER_PTV 0x0
#define TIMER_PCR 0x4

static void __iomem *timer_reg_base = (void __iomem *) IO_TO_VIRT(TEGRA_TMR1_BASE);

#define timer_writel(value, reg) \
	__raw_writel(value, (u32)timer_reg_base + (reg))
#define timer_readl(reg) \
	__raw_readl((u32)timer_reg_base + (reg))

irqreturn_t tegra_timer_interrupt(int irq, void *dev_id, struct cpu_user_regs *regs)
{
	timer_writel(1<<30, TIMER3_BASE + TIMER_PCR);
	printk("+");
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

	printk("rate = 0x%x\n", rate);
	switch (rate) {
	case 12000000:
		timer_writel(0x000b, TIMERUS_USEC_CFG);
		break;
	case 13000000:
		timer_writel(0x000c, TIMERUS_USEC_CFG);
		break;
	case 19200000:
		timer_writel(0x045f, TIMERUS_USEC_CFG);
		break;
	case 26000000:
		timer_writel(0x0019, TIMERUS_USEC_CFG);
		break;
	default:
		printk("Unknown clock rate");
	}

	timer_writel(0, TIMER3_BASE + TIMER_PTV);
	reg = 0xC0000000 | ((1000000/HZ)-1);
	setup_irq(INT_TMR3, &tegra_timer_irq);
	timer_writel(reg, TIMER3_BASE + TIMER_PTV);
}
