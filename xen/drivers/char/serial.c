/*
 * serial.c - framework for serial device drivers.
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 * Copyright (c) 2003-2005, K A Fraser
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
#include <xen/list.h>
#include <xen/spinlock.h>
#include <xen/config.h>
#include <xen/serial.h>
#include <xen/console.h>

static LIST_HEAD(serial_list);

static spinlock_t serial_lock = SPIN_LOCK_UNLOCKED;

void serial_rx_interrupt(struct serial_port *port,
                         struct cpu_user_regs *regs)
{
   char c;
   unsigned long flags;
   console_rx_handler fn = NULL;

   spin_lock_irqsave(&port->rx_lock, flags);

   if (port->driver->getc(port, &c)) {
      if (port->rx != NULL) {
         fn = port->rx;
      } else if ((port->rxbufp - port->rxbufc) != SERIAL_RXBUFSZ) {
         port->rxbuf[MASK_SERIAL_RXBUF_IDX(port->rxbufp++)] = c;
      }
   }

   spin_unlock_irqrestore(&port->rx_lock, flags);

   if (fn != NULL) {
      (*fn)(c & 0x7f, regs);
   }
}

void serial_tx_interrupt(struct serial_port *port,
                         struct cpu_user_regs *regs)
{
   int i;
   unsigned long flags;

   local_irq_save(flags);

   /*
    * Avoid spinning for a long time: if there is a long-term lock holder
    * then we know that they'll be stuffing bytes into the transmitter which
    * will therefore not be empty for long.
    */
   while (!spin_trylock(&port->tx_lock)) {
      if (!port->driver->tx_empty(port)) {
         spin_unlock_irqrestore(&port->tx_lock, flags);
         return;
      }
      cpu_relax();
   }

   if (port->driver->tx_empty(port)) {
      for (i = 0; i < port->tx_fifo_size; i++) {
         if (port->txbufc == port->txbufp) {
            break;
         }
         port->driver->putc(
            port, port->txbuf[MASK_SERIAL_TXBUF_IDX(port->txbufc++)]);
      }
   }

   spin_unlock_irqrestore(&port->tx_lock, flags);
}

static void __serial_putc(struct serial_port *port,
                          char c)
{
   int i;

   if ((port->txbuf != NULL) && !port->sync) {

      /* Interrupt-driven (asynchronous) transmitter. */
      if ((port->txbufp - port->txbufc) == SERIAL_TXBUFSZ) {

         /* Buffer is full: we spin, but could alternatively drop chars. */
         while (!port->driver->tx_empty(port)) {
            cpu_relax();
         }
         for (i = 0; i < port->tx_fifo_size; i++) {
            port->driver->putc(
               port, port->txbuf[MASK_SERIAL_TXBUF_IDX(port->txbufc++)]);
         }
         port->txbuf[MASK_SERIAL_TXBUF_IDX(port->txbufp++)] = c;
      }
      else if (((port->txbufp - port->txbufc) == 0) &&
               port->driver->tx_empty(port)) {

         /* Buffer and UART FIFO are both empty. */
         port->driver->putc(port, c);
      } else {

         /* Normal case: buffer the character. */
         port->txbuf[MASK_SERIAL_TXBUF_IDX(port->txbufp++)] = c;
      }
   } else if (port->driver->tx_empty) {

      /* Synchronous finite-capacity transmitter. */
      while (!port->driver->tx_empty(port)) {
         cpu_relax();
      }

      port->driver->putc(port, c);
   } else {

      /* Simple synchronous transmitter. */
      port->driver->putc(port, c);
   }
}

void serial_putc(struct serial_port *port, char c)
{
   unsigned long flags;

   spin_lock_irqsave(&port->tx_lock, flags);

   if (c == '\n' && port->flags & SERIAL_COOKED) {
      __serial_putc(port, '\r');
   }

   __serial_putc(port, c);

   spin_unlock_irqrestore(&port->tx_lock, flags);
}

void serial_puts(struct serial_port *port,
                 const char *s)
{
   unsigned long flags;
   char c;

   spin_lock_irqsave(&port->tx_lock, flags);

   while ((c = *s++) != '\0') {
      if (c == '\n' && port->flags & SERIAL_COOKED) {
         __serial_putc(port, '\r');
      }

      __serial_putc(port, c);
   }

   spin_unlock_irqrestore(&port->tx_lock, flags);
}

char serial_getc(struct serial_port *port)
{
   char c;
   unsigned long flags;

   for (;;) {
      spin_lock_irqsave(&port->rx_lock, flags);

      if (port->rxbufp != port->rxbufc) {
         c = port->rxbuf[MASK_SERIAL_RXBUF_IDX(port->rxbufc++)];
         spin_unlock_irqrestore(&port->rx_lock, flags);
         break;
      }

      if (port->driver->getc(port, &c)) {
         spin_unlock_irqrestore(&port->rx_lock, flags);
         break;
      }

      spin_unlock_irqrestore(&port->rx_lock, flags);

      cpu_relax();
   }

   return c & 0x7f;
}

void serial_con_set_rx_handler(struct serial_port *port,
                               console_rx_handler handler)
{
   unsigned long flags;

   spin_lock_irqsave(&port->rx_lock, flags);

   if (port->rx != NULL) {
      goto out;
   }

   port->rx = handler;

out:
   spin_unlock_irqrestore(&port->rx_lock, flags);
}

void serial_con_putc(struct console_info *con, char c)
{
   struct serial_port *port = con->private;

   serial_putc(port, c);
}

void serial_con_puts(struct console_info *con, const char *s)
{
   struct serial_port *port = con->private;

   serial_puts(port, s);
}

bool serial_con_can_write(struct console_info *con)
{
   struct serial_port *port = con->private;

   return (SERIAL_TXBUFSZ - (port->txbufp - port->txbufc)) < (SERIAL_TXBUFSZ / 2) ? false : true;
}

void serial_con_start_sync(struct console_info *con)
{
   unsigned long flags;
   struct serial_port *port = con->private;

   spin_lock_irqsave(&port->tx_lock, flags);

   if (port->sync++ == 0) {
      while ((port->txbufp - port->txbufc) != 0) {
         while (!port->driver->tx_empty(port)) {
            cpu_relax();
         }
         port->driver->putc(
            port, port->txbuf[MASK_SERIAL_TXBUF_IDX(port->txbufc++)]);
      }
   }

   spin_unlock_irqrestore(&port->tx_lock, flags);
}

void serial_con_end_sync(struct console_info *con)
{
   unsigned long flags;
   struct serial_port *port = con->private;

   spin_lock_irqsave(&port->tx_lock, flags);
   port->sync--;
   spin_unlock_irqrestore(&port->tx_lock, flags);
}

void serial_con_force_unlock(struct console_info *con)
{
   struct serial_port *port = con->private;

   port->rx_lock = SPIN_LOCK_UNLOCKED;
   port->tx_lock = SPIN_LOCK_UNLOCKED;

   serial_con_start_sync(con);
}

static struct console_info serial_console = {
   .name = "serial console",
   .set_rx_handler = serial_con_set_rx_handler,
   .putc = serial_con_putc,
   .puts = serial_con_puts,
   .can_write = serial_con_can_write,
   .start_sync = serial_con_start_sync,
   .end_sync = serial_con_end_sync,
   .force_unlock = serial_con_force_unlock,
};

void serial_register_port(struct serial_port *port)
{
   unsigned long flags;

   spin_lock_irqsave(&serial_lock, flags);
   port->rx_lock = SPIN_LOCK_UNLOCKED;
   port->tx_lock = SPIN_LOCK_UNLOCKED;
   list_add(&port->head, &serial_list);

   if (port->flags & SERIAL_CONSOLE &&
       serial_console.private == NULL) {
      serial_console.private = port;
      console_register(&serial_console);
   }
   spin_unlock_irqrestore(&serial_lock, flags);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
