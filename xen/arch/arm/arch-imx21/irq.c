#include <xen/config.h>
#include <asm/io.h>
#include <xen/init.h> // kcr
#include <xen/types.h> //
#include <asm/irq.h>

extern struct irqdesc irq_desc[NR_IRQS];
static struct irqchip imx21_internal_chip;
void set_irq_chip(unsigned int irq, struct irqchip *chip);

/*
 *
 * We simply use the ENABLE DISABLE registers inside of the IMX21
 * to turn on/off specific interrupts.  FIXME- We should
 * also add support for the accelerated interrupt controller
 * by putting offets to irq jump code in the appropriate
 * places.
 *
 */

#define INTENNUM_OFF              0x8
#define INTDISNUM_OFF             0xC

#define VA_AITC_BASE              IO_ADDRESS(IMX_AITC_BASE)
#define IMX21_AITC_INTDISNUM       (VA_AITC_BASE + INTDISNUM_OFF)
#define IMX21_AITC_INTENNUM        (VA_AITC_BASE + INTENNUM_OFF)

#define DEBUG_IRQ(fmt...)  do { } while (0)

static void imx21_mask_irq(unsigned int irq)
{
	__raw_writel(irq, IMX21_AITC_INTDISNUM);
}

static void imx21_unmask_irq(unsigned int irq)
{
	__raw_writel(irq, IMX21_AITC_INTENNUM);
}

static int imx21_gpio_irq_type(unsigned int _irq, unsigned int type)
{
	unsigned int irq_type = 0, irq, reg, bit;

	irq = _irq - IRQ_GPIOA(0);
	reg = irq >> 5;
	bit = 1 << (irq % 32);

	GIUS(reg) |= bit;
	DDIR(reg) &= ~(bit);

	DEBUG_IRQ("setting type of irq %d to ", _irq);

	if (type & __IRQT_RISEDGE) {
		DEBUG_IRQ("rising edges\n");
		irq_type = 0x0;
	}
	if (type & __IRQT_FALEDGE) {
		DEBUG_IRQ("falling edges\n");
		irq_type = 0x1;
	}
	if (type & __IRQT_LOWLVL) {
		DEBUG_IRQ("low level\n");
		irq_type = 0x3;
	}
	if (type & __IRQT_HIGHLVL) {
		DEBUG_IRQ("high level\n");
		irq_type = 0x2;
	}

	if (irq % 32 < 16) {
		ICR1(reg) = (ICR1(reg) & ~(0x3 << ((irq % 16) * 2))) |
			(irq_type << ((irq % 16) * 2));
	} else {
		ICR2(reg) = (ICR2(reg) & ~(0x3 << ((irq % 16) * 2))) |
			(irq_type << ((irq % 16) * 2));
	}

	return 0;

}

static void imx21_gpio_ack_irq(unsigned int irq)
{
	DEBUG_IRQ("%s: irq %d  isr %d\n", __FUNCTION__, irq,
			IRQ_TO_REG(irq));
	ISR(IRQ_TO_REG(irq)) |= 1 << ((irq - IRQ_GPIOA(0)) % 32);
}

static void imx21_gpio_mask_irq(unsigned int irq)
{
	DEBUG_IRQ("%s: irq %d\n", __FUNCTION__, irq);
	IMR(IRQ_TO_REG(irq)) &= ~( 1 << ((irq - IRQ_GPIOA(0)) % 32));
}

static void imx21_gpio_unmask_irq(unsigned int irq)
{
	DEBUG_IRQ("%s: irq %d\n", __FUNCTION__, irq);
	IMR(IRQ_TO_REG(irq)) |= 1 << ((irq - IRQ_GPIOA(0)) % 32);
	PMASK = PMASK | (1 << IRQ_TO_REG(irq));
}

static void imx21_gpio_handler(unsigned int irq_unused, struct irqdesc *desc,
		struct cpu_user_regs *regs)
{
	unsigned int mask;
	unsigned int port;
	unsigned int irq_base;
	unsigned int irq = 0;

	for (port = 0; port < 6; port++) {
		if (ISR(port) & IMR(port)) {
			break;
		}
	}

	mask = ISR(port);
	irq_base = IRQ_GPIOA(0) + (port * 32);

	desc = irq_desc + irq_base;
	while (mask) {
		if (mask & 1) {
			DEBUG_IRQ("handling irq %d (port %d)\n", irq, port);
			desc->handle(irq + irq_base, desc, regs);
			ISR(port) = (1 << irq);
		}
		irq++;
		desc++;
		mask >>= 1;
	}
}

static struct irqchip imx21_internal_chip = {
	.trigger_type = "level",
	.ack = imx21_mask_irq,
	.mask = imx21_mask_irq,
	.unmask = imx21_unmask_irq,
};


static struct irqchip imx21_gpio_chip = {
	.trigger_type = "edge",
	.ack = imx21_gpio_ack_irq,
	.mask = imx21_gpio_mask_irq,
	.unmask = imx21_gpio_unmask_irq,
	.set_type = imx21_gpio_irq_type,
};


void imx21_irq_init(void)
{
	unsigned int irq;	
	/* Mask all interrupts initially */
	IMR(0) = 0;
	IMR(1) = 0;
	IMR(2) = 0;
	IMR(3) = 0;
	IMR(4) = 0;
	IMR(5) = 0;
	
	for (irq = 0; irq < IMX_IRQS; irq++) {
		set_irq_chip(irq, &imx21_internal_chip);
		set_irq_handler(irq, level_irq_handler);
		set_irq_flags(irq, IRQF_VALID);
	}

	for (irq = IRQ_GPIOA(0); irq < IRQ_GPIOF(32); irq++) {
		set_irq_chip(irq, &imx21_gpio_chip);
		set_irq_handler(irq, edge_irq_handler);
		set_irq_flags(irq, IRQF_VALID | IRQF_TRIGGER_PROBE);
	}

	set_irq_chained_handler(INT_GPIO, imx21_gpio_handler);

	/* Disable all interrupts initially. */
	/* In IMX21 this is done in the bootloader. */
}

