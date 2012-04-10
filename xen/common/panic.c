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
#include <xen/config.h>
#include <xen/string.h>
#include <xen/errno.h>
#include <xen/lib.h>
#include <xen/smp.h>
#include <public/xen.h>
#include <xen/console.h>
#include <xen/delay.h>
#include <asm/time.h>

void __weak debugger_trap_immediate(void)
{
}

void panic(const char *fmt, ...)
{
	va_list args;
	char buf[128];
	unsigned long flags;
	static spinlock_t lock = SPIN_LOCK_UNLOCKED;
	extern void machine_restart(char *);

	debugtrace_dump();

	va_start(args, fmt);
	(void)vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	/* Spit out multiline message in one go. */
	console_start_sync();
	spin_lock_irqsave(&lock, flags);
	printk("\n****************************************\n");
	printk("Panic on CPU %d:\n", smp_processor_id());
	printk(buf);
	printk("****************************************\n\n");
	spin_unlock_irqrestore(&lock, flags);

	debugger_trap_immediate();

	printk("Reboot in five seconds...\n");
	watchdog_disable();
	mdelay(5000);
	machine_restart(0);
}
