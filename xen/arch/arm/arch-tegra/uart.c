/*
 * uart.c
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

#include <xen/kernel.h>
#include <xen/spinlock.h>
#include <xen/serial.h>
#include <asm/platform.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/termbits.h>
#include <asm/serial_reg.h>
#include <asm/arch/hardware.h>

#define UART_SHIFT 2

static struct tegra_uart {
   void __iomem *base;
} tegra_uart;

struct serial_port tegra_port = {
#if defined(CONFIG_TEGRA_DEBUG_UARTA)
   .name = "uarta"
#elif defined(CONFIG_TEGRA_DEBUG_UARTB)
   .name = "uartb"
#elif defined(CONFIG_TEGRA_DEBUG_UARTC)
   .name = "uartc"
#elif defined(CONFIG_TEGRA_DEBUG_UARTD)
   .name = "uartd"
#elif defined(CONFIG_TEGRA_DEBUG_UARTE)
   .name = "uarte"
#endif
};

static inline unsigned char tegra_uart_read(struct tegra_uart *uart, int offset)
{
   return readb(uart->base + (offset << UART_SHIFT));
}

static inline void tegra_uart_write(struct tegra_uart *uart, unsigned char val, int offset)
{
   writeb(val, uart->base + (offset << UART_SHIFT));
}

static int tegra_uart_tx_empty(struct serial_port *port)
{
   struct tegra_uart *uart = port->uart;
   unsigned long lsr = tegra_uart_read(uart, UART_LSR);
   if (lsr & UART_LSR_THRE)
      return 1;
   return 0;
}

static void tegra_uart_putc(struct serial_port *port, char c)
{
   struct tegra_uart *uart = port->uart;

   tegra_uart_write(uart, c, UART_TX);
   if (c == '\n') {
      while(!tegra_uart_tx_empty(port))
         cpu_relax();

      tegra_uart_putc(port, '\r');
   }
}

static int tegra_uart_getc(struct serial_port *port, char *pc)
{
   struct tegra_uart *uart = port->uart;
   unsigned long lsr = tegra_uart_read(uart, UART_LSR);
   if (lsr & UART_LSR_DR) {
      *pc = tegra_uart_read(uart, UART_RX);
      return 1;
   }
   return 0;
}

static struct uart_driver tegra_uart_driver = {
   .tx_empty     = tegra_uart_tx_empty,
   .putc         = tegra_uart_putc,
   .getc         = tegra_uart_getc
};

void tegra_uart_init()
{
   tegra_port.uart = &tegra_uart;
   tegra_port.driver = &tegra_uart_driver;

   tegra_uart.base = (void __iomem *) IO_TO_VIRT(TEGRA_DEBUG_UART_BASE);
   if (tegra_uart_read(&tegra_uart, UART_LSR) & UART_LSR_DR)
      tegra_uart_read(&tegra_uart, UART_RX);

   tegra_port.flags |= SERIAL_CONSOLE;
   serial_register_port(&tegra_port);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
