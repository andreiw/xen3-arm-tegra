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
unsigned long imx_gettimeoffset(void)
{
        unsigned long ticks;
        unsigned long results;

        /*
         * Get the current number of ticks.  Note that there is a race
         * condition between us reading the timer and checking for
         * an interrupt.  We get around this by ensuring that the
         * counter has not reloaded between our two reads.
         */
	ticks = IMX_TCN(TIMER_BASE);

        /*
         * Interrupt pending?  If so, we've reloaded once already.
         */

        if (IMX_TSTAT(TIMER_BASE) & TSTAT_COMP) {
                ticks += LATCH;
        }

        /* 
         * Convert the ticks to usecs
         */
        results = (1000000 / CLOCK_TICK_RATE) * ticks;
        return results;
}

irqreturn_t imx_timer_interrupt(int irq, void *dev_id, struct cpu_user_regs *regs)
{
	/* clear the interrupt */
	if (IMX_TSTAT(TIMER_BASE))
		IMX_TSTAT(TIMER_BASE) = TSTAT_CAPT | TSTAT_COMP;

	// jiffies counting is necessary.
	timer_tick(1);

#if 0
	raise_softirq(TIMER_SOFTIRQ);
#endif
	return IRQ_HANDLED;
}

static struct irqaction imx_timer_irq = {
	.name		= "i.MX Timer Tick",
	.dev_id		= NULL,
	.handler	= imx_timer_interrupt
};

void imx_timer_init(void)
{
	/*
	 * Initialise to a known state (all timers off, and timing reset)
	 */
	IMX_TCTL(TIMER_BASE) = 0;
	IMX_TPRER(TIMER_BASE) = 0;
	IMX_TCMP(TIMER_BASE) = LATCH-1;

	IMX_TCTL(TIMER_BASE) = TCTL_CLK_32 | TCTL_IRQEN | TCTL_TEN; 
	/*
	 * Make irqs happen for the system timer
	 */
	setup_irq(TIM1_INT, &imx_timer_irq); 
}
