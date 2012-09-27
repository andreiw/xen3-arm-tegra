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
#include <xen/sched.h>
#include <xen/mm.h>
#include <xen/compile.h>
#include <xen/console.h>
#include <xen/serial.h>
#include <xen/delay.h>
#include <public/version.h>
#include <public/sched.h>

#include <asm/trap.h>
#include <asm/pt.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <asm/cpu-ops.h>
#include <asm/platform.h>
#include <asm/fiqdb.h>
#include <asm/atag.h>
#include <asm/irq.h>
#include <xen/bv.h>

#ifdef CONFIG_GCOV_XEN
#include <xen/gcov.h>
#endif

#define DOM_CREATE_SUCCESS 1
#define DOM_CREATE_FAIL    0

const char BANNER[] =
   "Xen/ARMv7 virtual machine monitor for ARM architecture\n" \
   "Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>\n" \
   "Copyright (C) 2007 Samsung Electronics Co, Ltd. All Rights Reserved.\n";

struct vcpu *idle_vcpu[NR_CPUS];

struct domain *idle_domain;
struct meminfo system_memory = {0,};

unsigned long pages_m_phys_end;

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

static void memory_init(struct bv *boot_volume)
{
   unsigned long i, s, e;
   unsigned long pages_m_phys_start;
   unsigned long phys_lowest_pa;
   unsigned long phys_highest_pa;
   unsigned long nr_pages;

   /* set page table base */
   idle_pgd = (pde_t *) (ma_to_va(cpu_get_ttb()));

   /*
    * Memory holes will be reserved during
    * init_boot_pages().
    */
   min_page = find_lowest_pfn(&system_memory);
   max_page = find_highest_pfn(&system_memory);
   phys_lowest_pa = min_page << PAGE_SHIFT;
   phys_highest_pa = max_page << PAGE_SHIFT;

   /* Initialise boot-time allocator with all RAM situated after modules. */
   frame_table = (struct page_info *)(round_pgup(((unsigned long)(&_end))));
   nr_pages = PFN_UP((max_page - min_page) * sizeof(struct page_info));
   memset(frame_table, 0, nr_pages << PAGE_SHIFT);

   pages_m_phys_start = boot_allocator_init(va_to_ma(frame_table) + (nr_pages << PAGE_SHIFT));
   pages_m_phys_end = va_to_ma(DIRECTMAP_VIRT_END);

   printk("Virtual memory map:\n");
   printk("=================================================\n");
   printk("SPECIAL_VIRT_START:    0x%08x\n", SPECIAL_VIRT_START);
   printk("DIRECTMAP_VIRT_END:    0x%08x (PA 0x%08x)\n",
          DIRECTMAP_VIRT_END,
          va_to_ma(DIRECTMAP_VIRT_END));
   printk("M pages end:           0x%08x (PA 0x%08x)\n",
          ma_to_va(pages_m_phys_end),
          pages_m_phys_end);
   printk("M pages start:         0x%08x (PA 0x%08x)\n",
          ma_to_va(pages_m_phys_start),
          pages_m_phys_start);
   printk("Frame table end:       0x%08x (PA 0x%08x)\n",
          frame_table, va_to_ma(frame_table));
   printk("Frame table start:     0x%08x (PA 0x%08x)\n",
          (unsigned) frame_table + (nr_pages << PAGE_SHIFT),
          va_to_ma((unsigned) frame_table + (nr_pages << PAGE_SHIFT)));
   printk("Kernel ends at:        0x%08x (PA 0x%08x)\n", &_end, va_to_ma(&_end));
   printk("Kernel linked at:      0x%08x (PA 0x%08x)\n", &_start, va_to_ma(&_start));
   printk("Idle page tables:      0x%08x (PA 0x%08x)\n", idle_pgd, cpu_get_ttb());
   printk("DIRECTMAP_VIRT_START:  0x%08x\n", DIRECTMAP_VIRT_START);
   printk("MAPCACHE_VIRT_START:   0x%08x\n", MAPCACHE_VIRT_START);
   printk("SHARED_INFO_BASE:      0x%08x\n", SHARED_INFO_BASE);
   printk("PAGE_TABLE_VIRT_START: 0x%08x\n", PAGE_TABLE_VIRT_START);
   printk("IOREMAP_VIRT_START:    0x%08x\n", IOREMAP_VIRT_START);
   printk("HYPERVISOR_VIRT_START: 0x%08x\n", HYPERVISOR_VIRT_START);

   /* Initialize boot allocator with available RAM. */
   for (i = 0; i < system_memory.nr_banks; i++) {
      s = system_memory.banks[i].base;
      e = s + system_memory.banks[i].size;
      printk("looking at bank %d\n", i);
      printk("        base - 0x%x\n", s);
      printk("        end  - 0x%x\n", e);

      if (s < pages_m_phys_start) {
         s = pages_m_phys_start;
      }
      if(e > phys_highest_pa) {
         e = phys_highest_pa;
      }

      boot_pages_init(s, e);
   }

   boot_pages_reserve(boot_volume->start, boot_volume->end);
   boot_allocator_end();

   for (i = 0; i < system_memory.nr_banks; i++) {
      s = system_memory.banks[i].base;
      e = s + system_memory.banks[i].size;
      if (s < pages_m_phys_start) {
         s = pages_m_phys_start;
      }
      if (e > pages_m_phys_end) {
         e = pages_m_phys_end;
      }
      if (s < e) {
         pages_m_init(s, e);
      }
   }
}


void trap_init(void)
{
   pte_t *table;
   struct page_info *pg;

   pg = pages_u_alloc1(NULL);
   BUG_ON(pg == NULL);
   printk("IVT page is PA 0x%08x\n", page_to_phys(pg));

   BUG_ON(pt_map(SPECIAL_VECTORS, page_to_phys(pg), PAGE_SIZE, PTE_ENTRY_HV) != 0);

   clear_page((void *) SPECIAL_VECTORS);
   memcpy((void *) SPECIAL_VECTORS, exception_vectors_table,
          sizeof(unsigned long) * 16);

   cpu_clean_cache_range(SPECIAL_VECTORS, SPECIAL_VECTORS + PAGE_SIZE);
   cpu_trap_init();
}

pte_t *shared_info_table;

static void shared_info_table_init(void)
{
   shared_info_table = pages_m_alloc1();
   clear_page(shared_info_table);

   cpu_flush_cache_range((unsigned long)shared_info_table, (unsigned long)shared_info_table + PAGE_SIZE);

   consistent_write((void *)&idle_pgd[PGD_IDX(SHARED_INFO_BASE)], pde_val(MK_PDE(va_to_ma(shared_info_table), PDE_TYPE_COARSE | PDE_DOMAIN_SUPERVISOR)));
}

void xd_test(void *context)
{
   s_time_t st = NOW();
   while (1) {
      if (NOW() - st > SECONDS(current->domain->domain_id * 2)) {
         printk("Hello from a Xen domain %u at %llu nsecs!\n", current->domain->domain_id,
                NOW());
         st = NOW();
      }
      sched_yield();
   }
}

void start_xen(void *unused)
{
   struct bv bv = INIT_BV_NONE;
   struct bv_file dom0_data = INIT_BV_FILE_NONE;
   struct bv_file initrd0_data = INIT_BV_FILE_NONE;
   char *cmdline = atag_cmdline();
   struct domain *xd, *xd2;

   /*
    * Command line arguments may override
    * platform initialization, so this
    * must be the first thing done.
    */
   cmdline_parse(cmdline);
   platform_setup();

   atag_initrd((u32 *) &bv.start, (u32 *) &bv.end);

   memory_init(&bv);
   pt_init((vaddr_t) idle_pgd);

   sort_extables();

   trap_init();

#ifdef CONFIG_FIQDB
   fiqdb_init();
#endif /* CONFIG_FIQDB */

#if CONFIG_GCOV_XEN
   gcov_core_init();
#endif

   timer_init();

   scheduler_init();

   arch_init_memory();

   mapcache_init();

   shared_info_table_init();

   subsystem_init();

   /*
    * Xen is now initialized.
    */
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
   printk(" Kernel command line: %S\n", cmdline);
   printk(" Boot volume: 0x%x-0x%x\n", bv.start, bv.end);

#ifdef UNIT_TEST
   /* Unit Test Example */
   embunit_test_example();
#endif

   idle_domain = domain_create(IDLE_DOMAIN_ID, 0, 0);

   BUG_ON(idle_domain == NULL);

   set_current(idle_domain->vcpu[0]);
   idle_vcpu[0] = current;
   idle_vcpu[0]->arch.guest_table = va_to_ma(&idle_pgd[0]);

   /* While we're not stable... */
   /* debugger_trap_immediate(); */

#if 0
   dom0 = domain_create(0, 0, 0);

   BUG_ON(dom0 == NULL);
   if (bv.start == bv.end)
      panic("No boot volume passed to Xen :-(\n");

   if (bv_find(&bv, "dom0", &dom0_data))
      panic("No dom0 kernel present in boot volume :-(\n");

   printk("Dom0 kernel 0x%x-0x%x\n", dom0_data.start, dom0_data.end);

   if (!bv_find(&bv, "initrd0", &initrd0_data))
      printk("Dom0 initrd 0x%x-0x%x\n",
             initrd0_data.start, initrd0_data.end);

   /*
    * Hardcoding stuff in.
    */
   if (construct_dom0(dom0,
                      12 * 1024 * 1024,
                      (u32) dom0_data.start,
                      dom0_data.end - dom0_data.start,
                      (u32) initrd0_data.start,
                      initrd0_data.end - initrd0_data.start,
                      NULL) != 0)
      panic("Could not prepare dom0\n");

   set_bit(_DOMF_privileged, &dom0->domain_flags);
   domain_unpause_by_systemcontroller(dom0);
#endif

   xd = xen_domain_create(1, xd_test, 0);
   BUG_ON(xd == NULL);
   domain_unpause_by_systemcontroller(xd);

   xd2 = xen_domain_create(2, xd_test, 0);
   BUG_ON(xd2 == NULL);
   domain_unpause_by_systemcontroller(xd2);


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
   /*    panic("Could not find the domain structure for DOM guest OS\n"); */
   /*    return DOM_CREATE_FAIL;  */
   /* } */

   /* dom->store_port = dom0_op->u.guest_image_info.store_port; */
   /* dom->console_port = dom0_op->u.guest_image_info.console_port; */

   /* if ( construct_guest_dom( dom, */
   /*       domain_addrs[domain_id].guest_memory_start, */
   /*       domain_addrs[domain_id].guest_memory_size, */
   /*       domain_addrs[domain_id].elf_image_address, */
   /*       domain_addrs[domain_id].elf_image_size, */
   /*       domain_addrs[domain_id].initrd_address, */
   /*       domain_addrs[domain_id].initrd_size, */
   /*       NULL) != 0)           // stack start */
   /* { */
   /*    put_domain(dom); */
   /*    panic("Could not set up DOM1 guest OS\n"); */
   /*    return DOM_CREATE_FAIL; */
   /* } */

   /* dom0_op->u.guest_image_info.store_mfn = dom->store_mfn; */
   /* dom0_op->u.guest_image_info.console_mfn = dom->console_mfn; */

   /* put_domain(dom); */
   /* return DOM_CREATE_SUCCESS; */
   return -ENXIO;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
