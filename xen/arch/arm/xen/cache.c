/*
 * cache.c
 *
 * Copyright (C) 2008 Samsung Electronics
 *          SungKwan Heo  <sk.heo@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of
 * Xen on ARM and the associated access control.
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


#include <xen/config.h>
#include <xen/sched.h>
#include <xen/softirq.h>
#include <asm/cache.h>

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
