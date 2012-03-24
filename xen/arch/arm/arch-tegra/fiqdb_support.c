/*
 * fiqdb_support.c
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
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

#include <xen/lib.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/fiqdb.h>
#include <xen/init.h>
#include <asm/termbits.h>
#include <asm/serial_reg.h>
#include <asm/arch/hardware.h>

#define UART_SHIFT 2
static void __iomem *uart_base = (void __iomem *)
	IO_TO_VIRT(TEGRA_DEBUG_UART_BASE);

static inline unsigned char uart_read(int offset)
{
	return readb(uart_base + (offset << UART_SHIFT));
}

static inline void uart_write(unsigned char val, int offset)
{
	writeb(val, uart_base + (offset << UART_SHIFT));
}

static void tegra_fiqdb_putc(char c)
{
	unsigned long lsr;
	do {
		lsr = uart_read(UART_LSR);
		cpu_relax();
	} 	while ((lsr & (UART_LSR_THRE)) == 0);
	uart_write (c, UART_TX);

	if(c == '\n')
		tegra_fiqdb_putc('\r');
}

static int tegra_fiqdb_getc(void)
{
	unsigned long lsr = uart_read(UART_LSR);
	if (lsr & UART_LSR_DR)
		return uart_read(UART_RX);
	return FIQDB_NO_CHAR;
}

int tegra_fiqdb_init(void)
{
	tegra_fiq_enable(TEGRA_DEBUG_UART_INT);
	return 0;
}

static struct platform_fiqdb tegra_fiqdb = {
	.init = tegra_fiqdb_init,
	.getc = tegra_fiqdb_getc,
	.putc = tegra_fiqdb_putc,
};

int __init tegra_fiqdb_register(void)
{
	if (uart_read(UART_LSR) & UART_LSR_DR)
		uart_read(UART_RX);

	/* enable rx and lsr interrupt */
	uart_write(UART_IER_RLSI | UART_IER_RDI, UART_IER);

	/* interrupt on every character */
	uart_write(0, UART_IIR);
	fiqdb_register(&tegra_fiqdb);
}
