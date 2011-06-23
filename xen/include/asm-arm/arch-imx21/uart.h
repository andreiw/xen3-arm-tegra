#ifndef __IMX21ADS_UART_H__
#define __IMX21ADS_UART_H__

/* These parity settings can be ORed directly into the LCR. */
#define PARITY_NONE     (0<<3)
#define PARITY_ODD      (1<<3)
#define PARITY_EVEN     (3<<3)
#define PARITY_MARK     (5<<3)
#define PARITY_SPACE    (7<<3)

/* Frequency of external clock source. This definition assumes PC platform. */
#define UART_CLOCK_HZ   1843200

struct imx21ads_uart{
        int             baud;
        int             data_bits;
        int             parity;
        int             stop_bits;
        int             irq;
        unsigned long   io_base;   /* I/O port or memory-mapped I/O address. */
	char *remapped_io_base;  /* Remapped virtual address of mmap I/O.  */
	/* UART with IRQ line: interrupt-driven I/O. */
	// struct irqaction irqaction;
	/* UART with no IRQ line: periodically-polled I/O. */
	// struct timer timer;
	unsigned int timeout_ms;
};

void imx21ads_uart_init(int index, struct ns16550_defaults *defaults);

#endif

