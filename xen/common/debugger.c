/*
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
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
#include <xen/kernel.h>
#include <xen/errno.h>
#include <xen/lib.h>
#include <xen/string.h>
#include <xen/debugger.h>

void debug_list_commands(debug_printf_cb printf_cb)
{
   debug_command_t *command;

   BUG_ON(!printf_cb);

   printf_cb("\nXen debugger commands:\n");
   for (command = &__debug_commands_start;
        command < &__debug_commands_end;
        command++) {
      printf_cb("%s - %s\n", command->name, command->description);
   }
}

int debug_handle_command(const char *cmd, debug_printf_cb printf_cb)
{
   char *found;
   debug_command_t *command;

   BUG_ON(!cmd);
   BUG_ON(!printf_cb);

   for (command = &__debug_commands_start;
        command < &__debug_commands_end;
        command++) {
      found = strstr(command->name, cmd);
      if (found == command->name) {
         command->cb(cmd + strlen(command->name), printf_cb);
         return 0;
      }
   }

   return -ENOENT;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
