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

typedef void (*debug_command_cb)(const char *cmd, debug_printf_cb print_cb);

typedef struct debug_command {
   debug_command_cb cb;
   const char       *name;
   const char       *description;
} debug_command_t;

#define DEBUG_COMMAND(name, cb, desc)                                   \
   static debug_command_t __debugcommand_##name  __debug_command = {    \
      cb,                                                               \
      #name,                                                            \
      desc                                                              \
   };                                                                   \

extern debug_command_t __debug_commands_start, __debug_commands_end;

int debug_handle_command(const char *cmd, debug_printf_cb printk);

void debugger_trap_immediate(void);

#endif

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
