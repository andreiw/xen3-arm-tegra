#include <xen/sched.h>
#include <xen/irq.h>
#include <xen/init.h>
#include <xen/softirq.h>
#include <xen/spinlock.h>
#include <asm/time.h>
#include <asm/irq.h>


/*
 * Returns number of us since last clock interrupt.  Note that interrupts
 * will have been disabled by do_gettimeoffset()
 */
unsigned long tegra_gettimeoffset(void)
{
        return 0;
}

irqreturn_t tegra_timer_interrupt(int irq, void *dev_id, struct cpu_user_regs *regs)
{
	return IRQ_HANDLED;
}

static struct irqaction tegra_timer_irq = {
	.name		= "Tegra2 Timer Tick",
	.dev_id		= NULL,
	.handler	= tegra_timer_interrupt
};

void tegra_timer_init(void)
{
}
