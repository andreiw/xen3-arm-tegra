/*
 * delay.S
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 * Copyright (C) 2010 Colin Cross <ccross@google.com>
 * Copyright (C) 2010 Google, Inc.
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

#include <xen/linkage.h>
#include <asm/asm-macros.h>
#include <asm/arch/hardware.h>

ENTRY(udelay)
	mov32 r3, (TEGRA_PPSB_VIRT + TEGRA_TMRUS_BASE - TEGRA_PPSB_PHYS)
	ldr r1, [r3]

	 /* r0 - usecs to wait
	  * r1 - initial value of the counter
	  */
loop:
	ldr r2, [r3]
	sub r2, r2, r1
	cmp r2, r0
	bls loop
	mov pc, lr
