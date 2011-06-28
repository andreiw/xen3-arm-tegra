/*
 * platform.c
 *
 * Copyright (C) 2011 Andrei Warkentin <andreiw@motorola.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <xen/spinlock.h>
#include <xen/lib.h>
#include <xen/serial.h>
#include <asm/platform.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/serial_reg.h>

static void tegra_platform_halt(int mode)
{
}

DECLARE_PLATFORM_OP(platform_halt, tegra_platform_halt);

static void tegra_memory_init(void)
{
	register_memory_bank(0x00000000, 1 * 1024 * 1024 * 1024);
}

unsigned long uart_base = IO_TO_VIRT (TEGRA_DEBUG_UART_BASE);
unsigned long uart_shift = 2;

static inline unsigned char uart_read(int offset)
{
	return readb(uart_base + (offset << uart_shift));
}

static inline void uart_write(unsigned char val, int offset)
{
	writeb(val, uart_base + (offset << uart_shift));
}

static void tegra_uart_putc(struct serial_port *port, char c)
{
	unsigned long lsr;
	do {
		lsr = uart_read (UART_LSR);
		barrier();
	} 	while ((lsr & (UART_LSR_TEMT | UART_LSR_THRE)) == 0);
	uart_write (c, UART_TX);

	if(c == '\n') {
	 	tegra_uart_putc(port, '\r');
	}
}

static int tegra_uart_getc(struct serial_port *port, char *pc)
{
	return 1;
}

static struct ns16550_defaults tegra_uart_params = {
	.baud      = BAUD_AUTO,
	.data_bits = 8,
	.parity    = 'n',
	.stop_bits = 1
};

static struct uart_driver tegra_uart_driver = {
	.putc = tegra_uart_putc,
	.getc = tegra_uart_getc
};

static void tegra_uart_init(void)
{
	serial_register_uart(0, &tegra_uart_driver, &tegra_uart_params);
}

static void tegra_sys_clk_init(void)
{
}

static void tegra_platform_setup(void)
{
	tegra_memory_init();
	tegra_sys_clk_init();
	tegra_uart_init();
	tegra_irq_init();
	tegra_timer_init();
}

DECLARE_PLATFORM_OP(platform_setup, tegra_platform_setup);

static void tegra_platform_query(struct query_data *query)
{
	switch(query->type) {
		case QUERY_MEMORY_DETAILS : break;
		case QUERY_CPU_DETAILS : break;
		default : break;
	};
}

DECLARE_PLATFORM_OP(platform_query, tegra_platform_query);
