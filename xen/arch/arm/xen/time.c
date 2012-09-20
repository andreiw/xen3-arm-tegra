/*
 * time.c 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          Jaemin Ryu <jm77.ryu@samsung.com>
 *          JooYoung Hwang  <jooyoung.hwang@samsung.com>
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
#include <xen/time.h>
#include <xen/sched.h>
#include <xen/event.h>
#include <xen/softirq.h>
#include <asm/current.h>
#include <asm/div64.h>
#include <asm/time.h>

#define INITIAL_JIFFIES 0UL;

u64 jiffies_64 __cacheline_aligned = INITIAL_JIFFIES;

void timer_tick(unsigned int ticks)
{
	struct vcpu *v   = current;
	unsigned int cpu = smp_processor_id();

	jiffies_64 += ticks;

	if (likely(is_vm_vcpu(v)))
	{
		send_timer_event(v);
	}

	raise_softirq(TIMER_SOFTIRQ);
}


/* Set clock to <secs,usecs> after 00:00:00 UTC, 1 January, 1970. */
void do_settime(unsigned long secs, unsigned long nsecs, u64 system_time_base)
{
}

#if 0
void init_domain_time(struct domain *d)
{
	spin_lock(&wc_lock);

	ASSERT(d);

	version_update_begin(&d->shared_info->wc_version);

	d->shared_info->wc_sec  = wc_sec;
	d->shared_info->wc_nsec = wc_nsec;
	version_update_end(&d->shared_info->wc_version);
	spin_unlock(&wc_lock);
}
#endif

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
