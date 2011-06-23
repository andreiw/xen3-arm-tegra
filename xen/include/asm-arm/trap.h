/*
 * trap.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
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
#ifndef __TRAP_H__
#define __TRAP_H__

#define EXCEPTION_RESET			(0)
#define EXCEPTION_UNDEF			(1)
#define EXCEPTION_SWI			(2)
#define EXCEPTION_PABT			(3)
#define EXCEPTION_DABT			(4)
#define EXCEPTION_RESERVED		(5)
#define EXCEPTION_IRQ			(6)
#define EXCEPTION_FIQ			(7)

#if 0
void trap_init(void);
#endif
#endif
