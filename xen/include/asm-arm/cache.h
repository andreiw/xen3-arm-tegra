/*
 * mm.c
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

#ifndef __ARM_CACHE_H__
#define __ARM_CACHE_H__

#include <xen/prefetch.h>

#define CACHE_TYPE(x)			(((x) >> 25) & 15)
#define CACHE_S(x)				((x) & (1 << 24))
#define CACHE_DSIZE(x)			(((x) >> 12) & 4095)    /* only if S=1 */
#define CACHE_ISIZE(x)			((x) & 4095)

#define CACHE_SIZE(y)			(((y) >> 6) & 7)
#define CACHE_ASSOCIATIVITY(y)	(((y) >> 3) & 7)
#define CACHE_M(y)				((y) & (1 << 2))
#define CACHE_LINE(y)			((y) & 3)

#define L1_CACHE_BYTES			32

#define CACHE_VIVT				(0x00000001)
#define CACHE_VIPT				(0x00000002)
#define CACHE_PIPT				(0x00000004)

#define INVL_ICACHE
#define INVL_DCACHE

static inline void cache_flush(unsigned long start, unsigned long end, unsigned int flags)
{
}

static inline void cache_invalidate(unsigned long start, unsigned long end, unsigned int flags)
{
}

static inline void cache_clean(unsigned long start, unsigned long end, unsigned int flags)
{
}

static inline void dump_cache(const char *prefix, int cpu, unsigned int cache)
{
	unsigned int mult = 2 + (CACHE_M(cache) ? 1 : 0);

	printk("CPU%u: %s: %d bytes, associativity %d, %d byte lines, %d sets\n",
		cpu, prefix,
		mult << (8 + CACHE_SIZE(cache)),
		(mult << CACHE_ASSOC(cache)) >> 1,
		8 << CACHE_LINE(cache),
		1 << (6 + CACHE_SIZE(cache) - CACHE_ASSOC(cache) -
		CACHE_LINE(cache)));
}

#endif
