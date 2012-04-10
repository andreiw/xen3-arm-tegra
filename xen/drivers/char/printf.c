/*
 * printf.c - simple console handling.
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

#include <stdarg.h>
#include <xen/config.h>
#include <xen/compile.h>
#include <xen/linkage.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/errno.h>
#include <xen/spinlock.h>
#include <xen/serial.h>
#include <xen/foreground.h>
#include <asm/io.h>
#include <public/xen.h>
#include <xen/sched.h>
#include <xen/console.h>
#include <asm/current.h>

asmlinkage long do_console_io(int cmd, int count, char *buffer)
{
	long rc;
	
	switch ( cmd ) {
		case CONSOLEIO_write:
			if (current->domain->domain_id == foreground_domain) {
				buffer[count] = '\0';
				__putstr(buffer);
				rc = 0;
			} else {
				buffer[count] = '\0';
				__putstr(buffer);
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

static char printk_prefix[16] = "";
static int sercon_handle = -1;

spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

/*
 * *****************************************************
 * *************** GENERIC CONSOLE I/O *****************
 * *****************************************************
 */
void __putstr(const char *str)
{
	serial_puts(sercon_handle, str);
}

void printf(const char *fmt, ...)
{
	static char   buf[1024];
	static int    start_of_line = 1;

	va_list       args;
	char         *p, *q;
	unsigned long flags;

	spin_lock_irqsave(&console_lock, flags);

	va_start(args, fmt);
	(void)vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	p = buf;
	while ( (q = strchr(p, '\n')) != NULL )
	{
		*q = '\0';
		if ( start_of_line )
			__putstr(printk_prefix);
		__putstr(p);
		__putstr("\n");
		start_of_line = 1;
		p = q + 1;
	}

	if ( *p != '\0' )
	{
		if ( start_of_line )
			__putstr(printk_prefix);
		__putstr(p);
		start_of_line = 0;
	}

	spin_unlock_irqrestore(&console_lock, flags);
}

void set_printk_prefix(const char *prefix)
{
	strcpy(printk_prefix, prefix);
}

void console_start_sync(void)
{
	/* No-op. */
}

void debugtrace_dump(void)
{
	/* No-op. */
}

void init_console(void)
{
	sercon_handle = 0;
	serial_init_preirq();
	set_printk_prefix("[XEN] ");
}
