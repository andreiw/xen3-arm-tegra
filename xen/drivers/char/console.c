/*
 * console.c - console handling.
 *
 * TODO: This needs to handle Dom0 I/O (read) as well.
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
#include <xen/config.h>
#include <xen/compile.h>
#include <xen/linkage.h>
#include <xen/spinlock.h>
#include <xen/serial.h>
#include <xen/list.h>
#include <xen/foreground.h>
#include <asm/io.h>
#include <public/xen.h>
#include <xen/sched.h>
#include <xen/console.h>
#include <asm/current.h>

#define XEN_PRINTK_PREFIX "[Xen] "

static LIST_HEAD(console_list);

spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

static void console_puts(const char *str)
{
   struct console_info *con;

   /* console lock is already taken. */

   list_for_each_entry(con, &console_list, head) {
      con->puts(con, str);
   }
}

asmlinkage long do_console_io(int cmd, int count, char *buffer)
{
   long rc;

   switch (cmd) {
   case CONSOLEIO_write:

      /* XXX: this needs to change... */
      if (current->domain->domain_id == foreground_domain) {
         buffer[count] = '\0';
         console_puts(buffer);
         rc = 0;
      } else {
         buffer[count] = '\0';
         console_puts(buffer);
         rc = 0;
      }
      break;

   case CONSOLEIO_read:
      rc = 0;
      break;
   default:
      rc = -ENOSYS;
      break;
   }

   return rc;
}

void printk(const char *fmt, ...)
{
   static char   buf[1024];
   static int    start_of_line = 1;

   va_list       args;
   char         *p, *q;
   unsigned long flags;

   spin_lock_irqsave(&console_lock, flags);

   va_start(args, fmt);
   (void) vsnprintf(buf, sizeof(buf), fmt, args);
   va_end(args);

   p = buf;
   while ((q = strchr(p, '\n')) != NULL) {
      *q = '\0';
      if (start_of_line) {
         console_puts(XEN_PRINTK_PREFIX);
      }
      console_puts(p);
      console_puts("\n");
      start_of_line = 1;
      p = q + 1;
   }

   if (*p != '\0') {
      if (start_of_line) {
         console_puts(XEN_PRINTK_PREFIX);
      }

      console_puts(p);
      start_of_line = 0;
   }

   spin_unlock_irqrestore(&console_lock, flags);
}


/*
 * Puts all consoles into a mode that is fully synchronous
 * and not interrupt driven.
 */
void console_start_sync(void)
{
   unsigned long flags;
   struct console_info *con;

   spin_lock_irqsave(&console_lock, flags);
   list_for_each_entry(con, &console_list, head) {
      con->start_sync(con);
   }
   spin_unlock_irqrestore(&console_lock, flags);
}


/*
 * Puts all consoles into a mode that may use interrupts.
 */
void console_end_sync(void)
{
   unsigned long flags;
   struct console_info *con;

   spin_lock_irqsave(&console_lock, flags);
   list_for_each_entry(con, &console_list, head) {
      con->end_sync(con);
   }
   spin_unlock_irqrestore(&console_lock, flags);
}


/*
 * Called in the crash context where the locks could be held,
 * hence they need to be broken, and hardware needs to be put
 * in a mode that doesn't require interrupt driven control.
 */
void console_force_unlock(void)
{
   unsigned long flags;
   struct console_info *con;

   spin_lock_irqsave(&console_lock, flags);
   list_for_each_entry(con, &console_list, head) {
      con->force_unlock(con);
      con->start_sync(con);
   }
   spin_unlock_irqrestore(&console_lock, flags);
}


void debugtrace_dump(void)
{
   /* No-op. */
}


/*
 * Register a console.
 */
void console_register(struct console_info *con)
{
   unsigned long flags;

   spin_lock_irqsave(&console_lock, flags);
   list_add(&con->head, &console_list);
   spin_unlock_irqrestore(&console_lock, flags);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
