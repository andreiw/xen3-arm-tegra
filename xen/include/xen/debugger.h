/*
 * Copyright (C) 2012 Andrei Warkentin <andrey.warkentin@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef XEN_DEBUGGER_H
#define XEN_DEBUGGER_H

#include <asm/debugger.h>

typedef void (*debug_printf_cb)(const char *fmt, ...);

struct debug_command;
typedef void (*debug_command_cb)(struct debug_command *cmd, const char *arg, debug_printf_cb print_cb);

struct debug_command {
   debug_command_cb cb;
   const char       *name;
   const char       *description;
};

#define DEBUG_COMMAND(name, cb, desc)                                   \
   static struct debug_command __debugcommand_##name  __debug_command = {    \
      cb,                                                               \
      #name,                                                            \
      desc                                                              \
   };                                                                   \

extern struct debug_command __debug_commands_start, __debug_commands_end;

int debug_handle_command(const char *cmd, debug_printf_cb printk);

void debugger_trap_immediate(void);

#endif

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
