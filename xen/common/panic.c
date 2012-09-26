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
 */

#include <xen/kernel.h>
#include <xen/config.h>
#include <xen/smp.h>
#include <public/xen.h>
#include <xen/console.h>
#include <xen/symbols.h>
#include <xen/delay.h>
#include <asm/time.h>
#include <asm/stacktrace.h>

void __weak debugger_trap_immediate(void)
{
}


static int panic_bt_report(struct stackframe *frame, void *d)
{
   const char *name;
   unsigned *depth = d;
   unsigned long offset, size;
   char namebuf[KSYM_NAME_LEN+1];
   char buffer[sizeof("%s+%#lx/%#lx [%s]") + KSYM_NAME_LEN +
               2*(BITS_PER_LONG*3/10) + 1];

   if (*depth) {
      name = symbols_lookup(frame->pc, &size, &offset, namebuf);

      printk("sp %08x fp %08x ", frame->sp, frame->fp);
      if (!name) {
         printk("pc %08x\n", frame->pc);
      } else {
         printk("pc %08x (%s+%#lx/%#lx)\n",
                frame->pc, name, offset, size);
      }
      *depth--;
      return 0;
   }
   printk("...\n");
   return *depth == 0;
}


void panic(const char *fmt, ...)
{
   va_list args;
   char buf[128];
   unsigned long flags;
   struct stackframe frame;
   unsigned bt_depth = 100;
   static spinlock_t lock = SPIN_LOCK_UNLOCKED;
   extern void machine_restart(char *);

   debugtrace_dump();

   va_start(args, fmt);
   vsnprintf(buf, sizeof(buf), fmt, args);
   va_end(args);

   /* Break locks and ensuring TX is synchronous. */
   console_force_unlock();
   spin_lock_irqsave(&lock, flags);

   printk("\n********************************************\n");
   printk("Panic on CPU %d:\n", smp_processor_id());
   printk("%s\n", buf);
   printk("********************************************\n\n");

   frame.fp = __builtin_frame_address(0);
   frame.sp = __builtin_frame_address(0);
   frame.pc = __builtin_return_address(0);
   walk_stackframe(&frame, panic_bt_report, &bt_depth);
   spin_unlock_irqrestore(&lock, flags);

   debugger_trap_immediate();

   printk("Reboot in five seconds...\n");
   watchdog_disable();
   mdelay(5000);
   machine_restart(0);
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
