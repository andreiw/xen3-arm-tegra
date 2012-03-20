/*
 * xensetup.c
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 * Copyright (C) 2008 Samsung Electronics
 * Copyright (C) 2008 JooYoung Hwang <jooyoung.hwang@samsung.com>
 * Copyright (C) 2008 Jaemin Ryu <jm77.ryu@samsung.com>
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
#include <xen/init.h>
#include <xen/sched.h>
#include <xen/mm.h>
#include <xen/compile.h>
#include <xen/console.h>
#include <xen/serial.h>
#include <xen/string.h>
#include <public/version.h>
#include <public/sched.h>
#include <security/acm/policy_conductor.h>
#include <security/acm/acm_hooks.h>
#include <security/ssm-xen/sra_func.h>
#include <asm/trap.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <asm/cpu-ops.h>
#include <asm/platform.h>
#include <asm/atag.h>
#include <xen/bv.h>

#ifdef CONFIG_GCOV_XEN
#include <xen/gcov.h>
#endif

#define DOM_CREATE_SUCCESS	1
#define DOM_CREATE_FAIL		0

const char BANNER[] =
	"Xen/ARMv7 virtual machine monitor for ARM architecture\n" \
	"Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>\n" \
	"Copyright (C) 2007 Samsung Electronics Co, Ltd. All Rights Reserved.\n";

struct vcpu *idle_vcpu[NR_CPUS];

struct domain *idle_domain;
struct meminfo system_memory = {0,};

unsigned long xenheap_phys_start;
unsigned long xenheap_phys_end;

cpumask_t cpu_online_map;

static void subsystem_init(void)
{
	initcall_t *call;

	for ( call = &__initcall_start; call < &__initcall_end; call++ )
	{
		(*call)();
	}
}

void arch_get_xen_caps(xen_capabilities_info_t info)
{
	char *p = info;

	p += sprintf(p, "xen-%d.%d-arm ", XEN_VERSION, XEN_SUBVERSION);

	*(p-1) = 0;

	BUG_ON((p - info) > sizeof(xen_capabilities_info_t));

}

static unsigned long find_lowest_pfn(struct meminfo *mi)
{
	int i;
	unsigned long start = 0xFFFFFFFF;

	for(i = 0; i < mi->nr_banks; i++) {
		struct memory_bank *bank = &mi->banks[i];

		if(bank->base < start) {
			start = bank->base;
		}
	}

	return start >> PAGE_SHIFT;
}

static unsigned long find_highest_pfn(struct meminfo *mi)
{
	int i;
	unsigned long end = 0;

	for(i = 0; i < mi->nr_banks; i++) {
		struct memory_bank *bank = &mi->banks[i];

		if(end < bank->base + bank->size) {
			end = bank->base + bank->size;
		}
	}

	return end >> PAGE_SHIFT;
}

static void memory_init()
{
	unsigned long nr_pages = 0;
	unsigned long i, s, e;
	unsigned long xen_pstart;
	unsigned long xen_pend;

	/* set page table base */
	idle_pgd = (pde_t *) (ma_to_va(cpu_get_ttb()));
	printk("TTB PA 0x%x\n", cpu_get_ttb());
	printk("idle_pgd VA 0x%x\n", idle_pgd);

	/*
	 * Memory holes will be reserved during
	 * init_boot_pages().
	 */
	min_page = find_lowest_pfn(&system_memory);
	max_page = find_highest_pfn(&system_memory);

	xen_pstart = min_page << PAGE_SHIFT;
	xen_pend = max_page << PAGE_SHIFT;
	printk("xen_pstart 0x%x\n", xen_pstart);
	printk("xen_pend 0x%x\n", xen_pend);

	/* Initialise boot-time allocator with all RAM situated after modules. */
	printk("_end 0x%x\n", &_end);
	printk("_end VA 0x%X\n", va_to_ma(&_end));
	frame_table = (struct page_info *)(round_pgup(((unsigned long)(&_end))));
	nr_pages = PFN_UP((max_page - min_page) * sizeof(struct page_info));
	printk("nr_pages needed for all page_infos = 0x%x\n", nr_pages);
	printk("frame table is at 0x%x-0x%x\n",
	       frame_table,
	       (unsigned) frame_table + (nr_pages << PAGE_SHIFT));

	memset(frame_table, 0, nr_pages << PAGE_SHIFT);

	xenheap_phys_start = init_boot_allocator(va_to_ma(frame_table) + (nr_pages << PAGE_SHIFT));
	xenheap_phys_end   = xenheap_phys_start + KERNEL_HEAP_SIZE;
	printk("xenheap_phys_start = 0x%x (VA 0x%x)\n", xenheap_phys_start, ma_to_va(xenheap_phys_start));
	printk("xenheap_phys_end = 0x%x (VA 0x%x)\n", xenheap_phys_end, ma_to_va(xenheap_phys_end));

	/* Initialise the DOM heap, skipping RAM holes. */
	nr_pages = 0;
	for ( i = 0; i < system_memory.nr_banks; i++ ) {
		nr_pages += system_memory.banks[i].size >> PAGE_SHIFT;

		/* Initialise boot heap, skipping Xen heap and dom0 modules. */
		s = system_memory.banks[i].base;
		e = s + system_memory.banks[i].size;
		printk("looking at bank %d\n", i);
		printk("        base - 0x%x\n", s);
		printk("        end  - 0x%x\n", e);

		if ( s < xenheap_phys_start )
			s = xenheap_phys_start;
		if( e > xen_pend )
			e = xen_pend;
		printk("calling init_boot_pages on 0x%x-0x%x\n", s, e);
		init_boot_pages(s, e);
	}

	total_pages = nr_pages;
	end_boot_allocator();

	/* Initialise the Xen heap, skipping RAM holes. */
	nr_pages = 0;
	for ( i = 0; i < system_memory.nr_banks; i++ ) {
		s = system_memory.banks[i].base;
		e = s + system_memory.banks[i].size;
		if ( s < xenheap_phys_start )
			s = xenheap_phys_start;
		if ( e > xenheap_phys_end )
			e = xenheap_phys_end;
		if ( s < e ) {
			nr_pages += (e - s) >> PAGE_SHIFT;
			init_xenheap_pages(s, e);
		}
	}
}

void trap_init(void)
{
	extern char exception_vectors_table[];
	pte_t *table;
	void *vectors;

	/* Create Mapping for Exception vector table */
	table = alloc_xenheap_page();
	clear_page(table);
	vectors = alloc_xenheap_page();
	clear_page(vectors);

	cpu_flush_cache_range((unsigned long)table, (unsigned long) table + PAGE_SIZE);

	consistent_write((void *)&table[PGT_IDX(VECTORS_BASE)], pte_val(MK_PTE(va_to_ma(vectors), PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_UNO_SRW)));
	consistent_write((void *)&idle_pgd[PGD_IDX(VECTORS_BASE)], pde_val(MK_PDE(va_to_ma(table), PDE_TYPE_COARSE | PDE_DOMAIN_HYPERVISOR)));

	memcpy((void *)VECTORS_BASE, exception_vectors_table, sizeof(unsigned long) * 16);

	cpu_flush_cache_range(VECTORS_BASE, VECTORS_BASE + PAGE_SIZE);
	cpu_trap_init();
}

pte_t *shared_info_table;

static void shared_info_table_init(void)
{
	shared_info_table = alloc_xenheap_page();
	clear_page(shared_info_table);

	cpu_flush_cache_range((unsigned long)shared_info_table, (unsigned long)shared_info_table + PAGE_SIZE);

	consistent_write((void *)&idle_pgd[PGD_IDX(SHARED_INFO_BASE)], pde_val(MK_PDE(va_to_ma(shared_info_table), PDE_TYPE_COARSE | PDE_DOMAIN_SUPERVISOR)));
}

void start_xen(void *unused)
{
	struct bv bv = INIT_BV_NONE;
	struct bv_file dom0_data = INIT_BV_FILE_NONE;
	struct bv_file initrd0_data = INIT_BV_FILE_NONE;
	char *cmdline = atag_cmdline();

	/*
	 * Command line arguments may override
	 * platform initialization, so this
	 * must be the first thing done.
	 */
	cmdline_parse(cmdline);
	platform_setup();

	printk(XEN_BANNER);
	printk(BANNER);
	printk(" http://www.cl.cam.ac.uk/netos/xen\n");
	printk(" University of Cambridge Computer Laboratory\n\n");
	printk(" Xen version %d.%d%s (%s@%s) (%s) %s\n",
	       XEN_VERSION, XEN_SUBVERSION, XEN_EXTRAVERSION,
	       XEN_COMPILE_BY, XEN_COMPILE_DOMAIN,
	       XEN_COMPILER, XEN_COMPILE_DATE);
	printk(" Platform: %s\n", XEN_PLATFORM);
	printk(" GIT SHA: %s\n", XEN_CHANGESET);
	printk(" Kernel command line: %s\n", cmdline);
	atag_initrd((u32 *) &bv.start, (u32 *) &bv.end);
	printk(" Boot volume: 0x%x-0x%x\n", bv.start, bv.end);

	memory_init();

	sort_extables();

	trap_init();

#if CONFIG_GCOV_XEN
	gcov_core_init();
#endif

#ifdef CONFIG_VMM_SECURITY
	if ( sra_init() != 0 )
	  PANIC("Error Secure Repository Agent initialization\n");
#endif

	timer_init();

	init_acm();

	scheduler_init();

	arch_init_memory();

	mapcache_init();

	shared_info_table_init();

	subsystem_init();

#ifdef UNIT_TEST
	/* Unit Test Example */
	embunit_test_example();
#endif

	idle_domain = domain_create(IDLE_DOMAIN_ID, 0);

	BUG_ON(idle_domain == NULL);

	set_current(idle_domain->vcpu[0]);
	idle_vcpu[0] = current;
	idle_vcpu[0]->arch.guest_table = va_to_ma(&idle_pgd[0]);

	dom0 = domain_create(0, 0);

	BUG_ON(dom0 == NULL);
	if (bv.start == bv.end)
		PANIC("No boot volume passed to Xen :-(\n");

	if (bv_find(&bv, "dom0", &dom0_data))
		PANIC("No dom0 kernel present in boot volume :-(\n");

	printk("Dom0 kernel 0x%x-0x%x\n", dom0_data.start, dom0_data.end);

	if (!bv_find(&bv, "initrd0", &initrd0_data))
		printk("Dom0 initrd 0x%x-0x%x\n",
		       initrd0_data.start, initrd0_data.end);

	/* { */
	/* 	local_irq_enable(); */
	/* 	while(1); */
	/* } */

	/*
	 * Hardcoding stuff in.
	 */
	if (construct_dom0(dom0,
			   align_up(xenheap_phys_end, 0x100000),
			   12 * 1024 * 1024,
			   (u32) dom0_data.start,
			   dom0_data.end - dom0_data.start,
			   (u32) initrd0_data.start,
			   initrd0_data.end - initrd0_data.start,
			   NULL) != 0)
		PANIC("Could not prepare dom0\n");

	set_bit(_DOMF_privileged, &dom0->domain_flags);
	domain_unpause_by_systemcontroller(dom0);
	local_irq_enable();
	startup_cpu_idle_loop();
}

int get_guest_domain_address( dom0_op_t * dom0_op)
{
	/* unsigned int domain_id; */
	/* unsigned int ret=0; */
	/* dom0_op_t * op = dom0_op; */
	
	/* domain_id = op->u.guest_image_info.domain; */

	/* /\* return guest domain loading physical address *\/ */
	/* op->u.guest_image_info.guest_image_address = domain_addrs[domain_id].elf_image_address; */
	/* op->u.guest_image_info.guest_image_size    = domain_addrs[domain_id].elf_image_size; */

	/* return ret; */
	return -ENXIO;
}

int create_guest_domain( dom0_op_t * dom0_op )
{
	/* unsigned int domain_id; */
	/* unsigned long guest_va; */
	/* struct domain *dom; */

	/* domain_id = dom0_op->u.guest_image_info.domain; */

	/* guest_va = dom0_op->u.guest_image_info.guest_image_address; */

	/* dom = find_domain_by_id(domain_id); */
	/* if ( dom == NULL ) */
	/* { */
	/* 	PANIC("Could not find the domain structure for DOM guest OS\n"); */
	/* 	return DOM_CREATE_FAIL;	 */
	/* } */

	/* dom->store_port = dom0_op->u.guest_image_info.store_port; */
	/* dom->console_port = dom0_op->u.guest_image_info.console_port; */
		
	/* if ( construct_guest_dom( dom, */
	/* 		domain_addrs[domain_id].guest_memory_start, */
	/* 		domain_addrs[domain_id].guest_memory_size, */
	/* 		domain_addrs[domain_id].elf_image_address, */
	/* 		domain_addrs[domain_id].elf_image_size, */
	/* 		domain_addrs[domain_id].initrd_address, */
	/* 		domain_addrs[domain_id].initrd_size, */
	/* 		NULL) != 0)           // stack start */
	/* { */
	/* 	put_domain(dom); */
	/* 	PANIC("Could not set up DOM1 guest OS\n"); */
	/* 	return DOM_CREATE_FAIL; */
	/* } */

	/* dom0_op->u.guest_image_info.store_mfn = dom->store_mfn; */
	/* dom0_op->u.guest_image_info.console_mfn = dom->console_mfn; */

	/* put_domain(dom); */
	/* return DOM_CREATE_SUCCESS; */
	return -ENXIO;
}
