/*
 * platform.c
 *
 * Copyright (C) 2011 Andrei Warkentin <andreiw@motorola.com>
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

#include <xen/spinlock.h>
#include <xen/lib.h>
#include <xen/serial.h>
#include <asm/platform.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/serial_reg.h>
#include <asm/arch/hardware.h>

void tegra_fiqdb_register();
void tegra_uart_init(int index, struct ns16550_defaults *defaults);

static void tegra_platform_halt(int mode)
{
}

DECLARE_PLATFORM_OP(platform_halt, tegra_platform_halt);

static void tegra_memory_init(void)
{
	register_memory_bank(0x00000000, 1 * 1024 * 1024 * 1024);
}

static void tegra_sys_clk_init(void)
{
}

static void tegra_platform_setup(void)
{
#ifdef CONFIG_TEGRA_FIQDB_SUPPORT
	tegra_fiqdb_register();
#endif /* CONFIG_TEGRA_FIQDB_SUPPORT */
	tegra_uart_init(0, NULL);
	init_console();

	tegra_memory_init();
	tegra_sys_clk_init();
	tegra_irq_init();
	tegra_timer_init();
}

DECLARE_PLATFORM_OP(platform_setup, tegra_platform_setup);

static void tegra_platform_query(struct query_data *query)
{
	switch(query->type) {
		case QUERY_MEMORY_DETAILS : break;
		case QUERY_CPU_DETAILS : break;
		default : break;
	};
}

DECLARE_PLATFORM_OP(platform_query, tegra_platform_query);
