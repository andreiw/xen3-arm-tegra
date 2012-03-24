/*
 * fiqdb.c
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

#include <xen/types.h>
#include <xen/errno.h>
#include <asm/fiqdb.h>
#include <asm/irq.h>

static struct platform_fiqdb *pfiqdb;

static void handle_fiq(struct fiq_handler *h, void *regs, void *svc_sp)
{
	char c = pfiqdb->getc();
	if (c == FIQDB_NO_CHAR)
		return;

	if (c == '\r')
		c = '\n';
	pfiqdb->putc(c);
}

struct fiq_handler fiq_handler = {
	.fiq = handle_fiq,
};

/*
 * Initializes the FIQ debugger.
 */
int fiqdb_init(void)
{
	int status;

	if (!pfiqdb) {
		printk("FIQ debugger not enabled\n");
		return -ENXIO;
        }
	status = pfiqdb->init();
	if (status)
		return status;

	fiq_register_handler(&fiq_handler);
	local_fiq_enable();
	printk("FIQ debugger enabled\n");
	return 0;
}

/*
 * Registers platform FIQDB operations with the debugger.
 */
void fiqdb_register(struct platform_fiqdb *platform_fiqdb)
{
	pfiqdb = platform_fiqdb;
}
