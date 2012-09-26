/*
 * serial.h - framework for serial device drivers.
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

#ifndef XEN_SERIAL_H
#define XEN_SERIAL_H

#include <xen/console.h>

struct cpu_user_regs;

/* Number of characters we buffer for a polling receiver. */
#define SERIAL_RXBUFSZ 32
#define MASK_SERIAL_RXBUF_IDX(_i) ((_i)&(SERIAL_RXBUFSZ-1))

/* Number of characters we buffer for an interrupt-driven transmitter. */
#define SERIAL_TXBUFSZ 16384
#define MASK_SERIAL_TXBUF_IDX(_i) ((_i)&(SERIAL_TXBUFSZ-1))

struct uart_driver;

struct serial_port {
   struct list_head    head;
#define SERIAL_COOKED  (1 << 1) /* Newline/carriage-return translation?    */
#define SERIAL_CONSOLE (1 << 2) /* Serial is used for xen console. */
   unsigned            flags;
   char               *name;

   /* Uart-driver parameters. */
   struct uart_driver *driver;
   void               *uart;
   /* Number of characters the port can hold for transmit. */
   int                 tx_fifo_size;
   /* Transmit data buffer (interrupt-driven uart). */
   char               *txbuf;
   unsigned int        txbufp, txbufc;
   /* Force synchronous transmit. */
   int                 sync;
   /* Receiver callback */
   console_rx_handler  rx;
   /* Receive data buffer (polling receivers). */
   char                rxbuf[SERIAL_RXBUFSZ];
   unsigned int        rxbufp, rxbufc;
   /* Serial I/O is concurrency-safe. */
   spinlock_t          rx_lock, tx_lock;
};

struct uart_driver {
    /* Transmit FIFO ready to receive up to @tx_fifo_size characters? */
    int  (*tx_empty)(struct serial_port *);
    /* Put a character onto the serial line. */
    void (*putc)(struct serial_port *, char);
    /* Get a character from the serial line: returns 0 if none available. */
    int  (*getc)(struct serial_port *, char *);
};

/* Registers a serial port. */
void serial_register_port(struct serial_port *port);

/* Transmit a single character via the specified COM port. */
void serial_putc(struct serial_port *port, char c);

/*
 * An alternative to registering a character-receive hook. This function
 * will not return until a character is available. It can safely be
 * called with interrupts disabled.
 */
char serial_getc(struct serial_port *port);

/*
 * Initialisation and helper functions for uart drivers.
 */
/* Process work in interrupt context. */
void serial_rx_interrupt(struct serial_port *port, struct cpu_user_regs *regs);
void serial_tx_interrupt(struct serial_port *port, struct cpu_user_regs *regs);

/*
 * Initialisers for individual uart drivers.
 */
/* NB. Any default value can be 0 if it is unknown and must be specified. */
struct ns16550_defaults {
    int baud;      /* default baud rate; BAUD_AUTO == pre-configured */
    int data_bits; /* default data bits (5, 6, 7 or 8) */
    int parity;    /* default parity (n, o, e, m or s) */
    int stop_bits; /* default stop bits (1 or 2) */
    int irq;       /* default irq */
    unsigned long io_base; /* default io_base address */
};

/* Baud rate was pre-configured before invoking the UART driver. */
#define BAUD_AUTO (-1)

#endif /* XEN_SERIAL_H */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
