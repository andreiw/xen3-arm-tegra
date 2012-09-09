/*
 * arch_domain.c
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JooYoung Hwang  <jooyoung.hwang@samsung.com>
 *          Jaemin Ryu		<jm77.ryu@samsung.com>
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

#include <stdarg.h>
#include <xen/config.h>
#include <xen/lib.h>
#include <xen/sched.h>
#include <xen/mm.h>
#include <xen/domain.h>
#include <xen/errno.h>
#include <xen/smp.h>
#include <xen/irq_cpustat.h>
#include <xen/softirq.h>
#include <asm/current.h>	
#include <asm/cpu-ops.h>
#include <asm/memory.h>

#define switch_mm(mm)	cpu_switch_ttb(mm)

void __switch_to(struct vcpu *, struct vcpu_guest_context *, struct vcpu_guest_context *);

#define switch_to(prev,next,last)                                       \
do {                                                                    \
         __switch_to(prev,&prev->arch.guest_context, &next->arch.guest_context);   \
} while (0)


unsigned long hypercall_create_continuation(unsigned int op,
        const char *format, ...)
{
	printk("hypercall_create_continuation: Not Yet.\n");

	return 0;
}

extern pte_t *shared_info_table;
int arch_domain_create(struct domain *d)
{
	struct shared_info *shared_info;

	if (is_vm_domain(d))
	{
		shared_info = alloc_xenheap_page();

		if ( shared_info == NULL )
		{
			printf("Shared Info allocation failed\n");
		
			return -ENOMEM;
		}

		memset(shared_info, 0, PAGE_SIZE);
		d->shared_info = SHARED_INFO_LIMIT - (PAGE_SIZE * (d->domain_id + 1));
		consistent_write(&shared_info_table[PGT_IDX((unsigned long)d->shared_info)], va_to_ma(shared_info) | PTE_FLAG_SHARED_INFO);

		share_xen_page_with_guest(mfn_to_page(va_to_ma(shared_info) >> PAGE_SHIFT), d, PGT_writable_page);

		d->arch.ioport_caps = rangeset_new(d, "I/O Ports", RANGESETF_prettyprint_hex);
		if ( d->arch.ioport_caps == NULL )
			goto fail_nomem;
	}
	
	return 0;
		
fail_nomem:
	free_xenheap_page(shared_info);

	return -ENOMEM;
}

void arch_domain_destroy(struct domain *d)
{
}

void free_perdomain_pt(struct domain *d)
{
	printk("free_perdomain_pt : Not Yet\n");
}

struct vcpu *alloc_vcpu_struct(struct domain *d, unsigned int vcpu_id)
{
    struct vcpu *v;

    if ( (v = xmalloc(struct vcpu)) == NULL )
        return NULL;

    memset(v, 0, sizeof(*v));

    return v;
}

void free_vcpu_struct(struct vcpu *v)
{
    BUG_ON(v->next_in_list != NULL);
    if ( v->vcpu_id != 0 )
        v->domain->vcpu[v->vcpu_id - 1]->next_in_list = NULL;
    xfree(v);
}

/* This is called by arch_final_setup_guest and do_boot_vcpu */
int arch_set_info_guest(struct vcpu *v, vcpu_guest_context_t *c)
{
	printk("arch_set_info_guest : Not Yet\n");

	return 0;
}

void domain_relinquish_memory(struct domain *d)
{
	printk("domain_relinquish_memory : Not Yet\n");
}

void dump_pageframe_info(struct domain *d)
{
	printk("dump_pageframe_info : Not Yet\n");
}

void context_switch(struct vcpu *prev, struct vcpu *next)
{
	ASSERT(prev != next);
	ASSERT(vcpu_runnable(next));

	if (is_vm_domain(next->domain))
	{
		write_ptbase(next);
	}

	context_saved(prev);
	switch_to(prev,next,prev);
}

void continue_running(struct vcpu *same)
{
	return ;
}

void sync_lazy_execstate_cpu(unsigned int cpu)
{
	printk("sync_lazy_execstate_cpu : Not Yet\n");
}

void sync_lazy_execstate_mask(cpumask_t mask)
{
	printk("sync_lazy_execstate_mask : Not Yet\n");
}

void sync_vcpu_execstate(struct vcpu *v)
{
	printk("sync_vcpu_execstate : Not Yet\n");
}

static void relinquish_memory(struct domain *d, struct list_head *list)
{
    struct list_head *ent;
    struct page_info  *page;
    unsigned long     x, y;

    /* Use a recursive lock, as we may enter 'free_domheap_page'. */
    spin_lock_recursive(&d->page_alloc_lock);

    ent = list->next;
    while ( ent != list )
    {
        page = list_entry(ent, struct page_info, list);

        /* Grab a reference to the page so it won't disappear from under us. */
        if ( unlikely(!get_page(page, d)) )
        {
            /* Couldn't get a reference -- someone is freeing this page. */
            ent = ent->next;
            continue;
        }

        if ( test_and_clear_bit(_PGT_pinned, &page->u.inuse.type_info) )
            put_page_and_type(page);

        if ( test_and_clear_bit(_PGC_allocated, &page->count_info) )
            put_page(page);

        /*
         * Forcibly invalidate base page tables at this point to break circular
         * 'linear page table' references. This is okay because MMU structures
         * are not shared across domains and this domain is now dead. Thus base
         * tables are not in use so a non-zero count means circular reference.
         */
        y = page->u.inuse.type_info;
        for ( ; ; )
        {
            x = y;
            if ( likely((x & (PGT_type_mask|PGT_validated)) !=
                        (PGT_page_directory|PGT_validated)) )
				break;

            y = cmpxchg(&page->u.inuse.type_info, x, x & ~PGT_validated);
            if ( likely(y == x) )
            {
				free_page_type(page, PGT_page_directory);

				/*
				 * Currently we do not perform page table freeing. So the page type for
				 * page directory page should be putted manually
				 */
				put_page_and_type(page);

                break;
            }
		}
        /* Follow the list chain and /then/ potentially free the page. */
        ent = ent->next;
        put_page(page);
    }

    spin_unlock_recursive(&d->page_alloc_lock);
}

void trace_domheap_pages(const char *caption, struct domain *d, struct list_head *list)
{
	struct list_head *ent;
	struct page_info  *page;
	unsigned long     x, y;

	printf("%s\n", caption);

	/* Use a recursive lock, as we may enter 'free_domheap_page'. */
	spin_lock_recursive(&d->page_alloc_lock);

	printf("remaining domain pages = 0x%x\n", d->tot_pages);
	
	ent = list->next;
	while ( ent != list )
	{
		page = list_entry(ent, struct page_info, list);
		printf("page mfn = 0x%x, count = 0x%x, type =  0x%x\n", page_to_mfn(page), page->count_info, page->u.inuse.type_info); 
		ent = ent->next;
	}

    spin_unlock_recursive(&d->page_alloc_lock);

	printf("page trace completed [%s]\n", caption);
}

void domain_relinquish_resources(struct domain *d)
{
	struct vcpu *v;
    unsigned long pfn;

	ASSERT( d );
	ASSERT( d != current->domain );

#if 0
	/* Drop the in-use references to page-table bases. */
    for_each_vcpu ( d, v )
    {
        if ( (pfn = pagetable_get_pfn(v->arch.guest_table)) != 0 )
        {
           
            put_page(mfn_to_page(pfn));

            v->arch.guest_table = mk_guest_table(0);
        }
    }
#endif

	/* Relinquish every page of memory. */
    relinquish_memory(d, &d->xenpage_list);
	relinquish_memory(d, &d->page_list);
}

static void continue_cpu_idle_loop(void)
{
	while(1) {
		local_irq_disable();
		cpu_idle();
		local_irq_enable();
		raise_softirq(SCHEDULE_SOFTIRQ);
		do_softirq();
	}
}

void startup_cpu_idle_loop(void)
{
	schedulers_start();

	continue_cpu_idle_loop();
}

long arch_do_vcpu_op(int cmd, struct vcpu *v, GUEST_HANDLE(void) arg)
{
	return -ENOSYS;
}

