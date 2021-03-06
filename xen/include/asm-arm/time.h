#ifndef __ASM_TIME_H__
#define __ASM_TIME_H__

#include <xen/cache.h>
#include <xen/softirq.h>
#include <asm/page.h>
#include <asm/current.h>
#include <asm/arch/config.h>

#ifndef HZ
#warning "HZ is not defined. Use default value 100"
#define HZ		100
#endif

#define watchdog_disable() ((void)0)
#define watchdog_enable()  ((void)0)

#define send_timer_event(v)	send_guest_virq(v, VIRQ_TIMER)

extern u64 jiffies_64;

/*
 *  Return nanoseconds from time of boot
 */
static inline s64 get_s_time(void)
{
	return (s64)(jiffies_64 * (1000000000 / HZ));
}

#endif
