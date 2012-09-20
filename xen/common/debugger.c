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
#include <xen/compile.h>
#include <xen/debugger.h>
#include <asm/page.h>
#include <asm/tlb.h>

void debug_list_commands(debug_printf_cb printf_cb)
{
   struct debug_command *command;

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
   char *rem;
   char *found;
   struct debug_command *command;

   BUG_ON(!cmd);
   BUG_ON(!printf_cb);

   for (command = &__debug_commands_start;
        command < &__debug_commands_end;
        command++) {
      found = strstr(cmd, command->name);
      if (found != NULL) {
         rem = found + strlen(command->name);
         if (*rem != ' ' &&
             *rem != '\0') {
            return -ENOENT;
         }

         if (*rem == ' ') {
            rem++;
         }
         command->cb(command, rem, printf_cb);
         return 0;
      }
   }

   return -ENOENT;
}


static void debug_version(struct debug_command *cmd,
                          const char *arg,
                          debug_printf_cb cb)
{
   cb("Xen version %d.%d%s (%s@%s) (%s) %s\n",
      XEN_VERSION, XEN_SUBVERSION,
      XEN_EXTRAVERSION,
      XEN_COMPILE_BY, XEN_COMPILE_DOMAIN,
      XEN_COMPILER, XEN_COMPILE_DATE);
   cb("Platform: %s\n", XEN_PLATFORM);
   cb("GIT SHA: %s\n", XEN_CHANGESET);
}


DEBUG_COMMAND(ver, debug_version, "hypervisor version");

static void debug_hexdump(struct debug_command *cmd,
                          const char *arg,
                          debug_printf_cb cb)
{
   const u8 *ptr;
   unsigned groupsize;
   unsigned i, linelen;
   unsigned rowsize = 32;
   unsigned len;
   unsigned remaining;
   unsigned char linebuf[32 * 3 + 2 + 32 + 1];

   if (sscanf(arg, "%u 0x%x@0x%x", &groupsize, &len, &ptr) != 3) {
      cb("Usage: %s 1|2|4|8 <hex size>@<hex VA address>\n", cmd->name);
      return;
   }

   remaining = len;

   for (i = 0; i < len; i += rowsize) {
      linelen = min(remaining, rowsize);
      remaining -= rowsize;
      cb("%p: ", ptr + i);
      if (!tlb_check((vaddr_t) (ptr + i),
                     (vaddr_t) (ptr + linelen),
                     true, false)) {
         cb("fault\n");
         break;
      }
      hex_dump_to_buffer(ptr + i, linelen, rowsize, groupsize,
                         linebuf, sizeof(linebuf), true);

      cb("%s\n", linebuf);
   }
}

DEBUG_COMMAND(hexdump, debug_hexdump, "hexdump VA");

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
