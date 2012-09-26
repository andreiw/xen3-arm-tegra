/*
 * console.h - console handling.
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef XEN_CONSOLE_H
#define XEN_CONSOLE_H

#include <xen/list.h>
#include <xen/spinlock.h>
#include <xen/guest_access.h>
#include <public/xen.h>

struct cpu_user_regs;
struct console_info;

typedef void (*console_rx_handler)(char, struct cpu_user_regs *);
typedef void (*console_ops_set_rx_handler)(struct console_info *con,
                                           console_rx_handler handler);
typedef void (*console_ops_puts)(struct console_info *con,
                                 const char *s);
typedef bool (*console_ops_can_write)(struct console_info *con);
typedef void (*console_ops_start_sync)(struct console_info *con);
typedef void (*console_ops_end_sync)(struct console_info *con);
typedef void (*console_ops_force_unlock)(struct console_info *con);

struct console_info {
   struct list_head           head;
   char                       *name;
   console_ops_set_rx_handler set_rx_handler;
   console_ops_puts           puts;
   console_ops_can_write      can_write;
   console_ops_start_sync     start_sync;
   console_ops_end_sync       end_sync;
   console_ops_force_unlock   force_unlock;
   void                      *private;
};

void console_register(struct console_info *con);

long read_console_ring(GUEST_HANDLE(char), u32 *, int);

void console_force_unlock(void);

void console_start_sync(void);
void console_end_sync(void);

#endif /* XEN_CONSOLE_H */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
