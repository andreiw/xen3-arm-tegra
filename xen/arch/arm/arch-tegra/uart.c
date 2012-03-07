/******************************************************************************
 * imx21ads_serial.c
 * 
 * Copyright (c) 2006-2008, J Y Hwang
 */

#include <xen/types.h>
#include <xen/config.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/spinlock.h>
#include <xen/serial.h> 
#include <asm/io.h>
#include <asm/termbits.h>	 
#include <asm/serial_reg.h>	
#include <asm/arch/imx-regs.h>
#include <asm/arch/uart.h>

/*
 * Configure serial port with a string <baud>,DPS,<io-base>,<irq>.
 * The tail of the string can be omitted if platform defaults are sufficient.
 * If the baud rate is pre-configured, perhaps by a bootloader, then 'auto'
 * can be specified in place of a numeric baud rate.
 */
static char opt_com1[30] = "", opt_com2[30] = "";
string_param("com1", opt_com1);
string_param("com2", opt_com2);

struct imx21ads_uart imx21ads_uart_com[2] = { { 0 } };

static int imx21ads_uart_tx_empty(struct serial_port *port)
{
	return 1;
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)
static int lsr_break_flag;

static inline imx21ads_uart_putc(struct serial_port *port, char c)
{
	while(UTS(UART_1) & UTS_TXFULL);

	UTXD(UART_1) = c;

	if(c == '\n') {
		imx21ads_uart_putc(port, '\r');
	}

//	printch(c);
}

static int imx21ads_uart_getc(struct serial_port *port, char *pc)
{
	return 1;
}

static void imx21ads_uart_init_preirq(struct serial_port *port)
{
        int cflag = CREAD | HUPCL | CLOCAL;
        unsigned cval;
	int quot;

        cflag |= B115200;
        cflag |= CS8;
	quot = 3000000 / 115200;

	cval = cflag & (CSIZE | CSTOPB);
	cval >>= 4;

        if (cflag & PARENB)
            cval |= UART_LCR_PARITY;
        if (!(cflag & PARODD))
            cval |= UART_LCR_EPAR;
}

static void imx21ads_uart_init_postirq(struct serial_port *port)
{
}

#define imx21ads_uart_endboot NULL

static struct uart_driver imx21ads_uart_driver = {
        .init_preirq  = imx21ads_uart_init_preirq,
        .init_postirq = imx21ads_uart_init_postirq,
        .endboot      = imx21ads_uart_endboot,
        .tx_empty     = imx21ads_uart_tx_empty,
        .putc         = imx21ads_uart_putc,
        .getc         = imx21ads_uart_getc
};

static int parse_parity_char(int c)
{
        switch ( c ) {
        case 'n':
                return PARITY_NONE;
        case 'o': 
                return PARITY_ODD;
        case 'e': 
                return PARITY_EVEN;
        case 'm': 
                return PARITY_MARK;
        case 's': 
                return PARITY_SPACE;
        }

        return 0;
}

#define PARSE_ERR(_f, _a...)                        \
        do {                                        \
            printk( "ERROR: " _f "\n" , ## _a );    \
            return;                                 \
        } while ( 0 )

static void imx21ads_uart_parse_port_config(struct imx21ads_uart *uart, char *conf)
{
        /* Register with generic serial driver. */
        serial_register_uart(uart - imx21ads_uart_com, &imx21ads_uart_driver, uart);
}

void imx21ads_uart_init(int index, struct ns16550_defaults *defaults)
{
        struct imx21ads_uart *uart = &imx21ads_uart_com[index];

        if ( (index < 0) || (index > 1) )
                return;

        if ( defaults != NULL ) {
                uart->baud      = defaults->baud;
                uart->data_bits = defaults->data_bits;
                uart->parity    = parse_parity_char(defaults->parity);
                uart->stop_bits = defaults->stop_bits;
                uart->irq       = defaults->irq;
                uart->io_base   = defaults->io_base;
        }

        imx21ads_uart_parse_port_config(uart, (index == 0) ? opt_com1 : opt_com2);
}

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
