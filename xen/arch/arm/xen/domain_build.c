/*
 * entry.S
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
 *
 * Copyright (C) 2012 Andrei Warkentin <andrey.warkentin@gmail.com>
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

#include <xen/compile.h>
#include <xen/sched.h>
#include <xen/elf.h>
#include <xen/domain.h>
#include <xen/mm.h>
#include <xen/errno.h>
#include <xen/iocap.h>
#include <xen/xmalloc.h>
#include <public/xen.h>
#include <public/version.h>

extern struct domain * idle_domain;

static const char *feature_names[XENFEAT_NR_SUBMAPS * 32] = {
	[XENFEAT_writable_page_tables]       = "writable_page_tables",
	[XENFEAT_auto_translated_physmap]    = "auto_translated_physmap",
	[XENFEAT_supervisor_mode_kernel]     = "supervisor_mode_kernel",
};

static void parse_features(
	const char *feats,
	uint32_t supported[XENFEAT_NR_SUBMAPS],
	uint32_t required[XENFEAT_NR_SUBMAPS])
{
	const char *end, *p;
	int i, req;

	if ((end = strchr(feats, ',')) == NULL)
		end = feats + strlen(feats);

	while (feats < end) {
		p = strchr(feats, '|');
		if ((p == NULL) || (p > end))
			p = end;

		req = (*feats == '!');
		if (req)
			feats++;

		for (i = 0; i < XENFEAT_NR_SUBMAPS * 32; i++) {
			if (feature_names[i] == NULL)
				continue;

			if (strncmp(feature_names[i], feats, p - feats) == 0) {
				set_bit(i, (unsigned long *)supported);
			if (req)
				set_bit(i, (unsigned long *)required);
			break;
		}
	}

	if (i == XENFEAT_NR_SUBMAPS * 32) {
		printk("Unknown kernel feature \"%.*s\".\n",
		(int)(p-feats), feats);
		if (req)
			panic("Domain 0 requires an unknown hypervisor feature.\n");
	}

	feats = p;
	if ( *feats == '|' )
		feats++;
	}
}

struct guest_map_track {
	unsigned long mpt_base;
	unsigned long mpt_end;
	unsigned long map_track;
};


#define DECLARE_MAP_TRACK(dsi)								\
		unsigned long map_track;							\
		unsigned long mpt_base;								\
		unsigned long mpt_end;								\
															\
		map_track = dsi->p_start;							\
		mpt_base = map_track + (dsi->v_end - dsi->v_start); \
		mpt_base = mpt_end = PGD_ALIGN(mpt_base);
															
#define NEXT_TRACK()										\
	({														\
		map_track;											\
	})

#define TOTAL_GUEST_TABLES()								\
	({														\
		((mpt_end - mpt_base) >> PAGE_SHIFT);				\
	})

#define ALLOC_GUEST_TABLE(order)							\
	({														\
		unsigned long i, p = mpt_end;						\
		for(i = 0; i < ( 1 << order); i++) {				\
			clear_page(mpt_end);							\
			mpt_end += PAGE_SIZE;							\
		}													\
		p;													\
	})

#define ALLOC_GUEST_PAGE()									\
	({														\
		unsigned long p = map_track;						\
		map_track += PAGE_SIZE;								\
		p;													\
	})

unsigned long build_guest_tables(struct vcpu *v, struct domain_setup_info *dsi)
{
	pde_t *pgd;
	pte_t *pgt;
	u32 pgd_idx;
	u32 pgt_idx;
	unsigned long pg;

#define PGT_ENTRIES_PER_PAGE	512

	DECLARE_MAP_TRACK(dsi);

	pgd = (pde_t *)ALLOC_GUEST_TABLE(2);
	memcpy(pgd, &idle_pgd[0], PGD_SIZE);

	pgd_idx = PGD_IDX(dsi->v_start);
	pgt_idx = PGT_IDX(dsi->v_start);

	do {
		pgt = (pte_t *)ALLOC_GUEST_TABLE(0);

		/* Two page tables per each page */
		pgd[pgd_idx++] = MK_PDE((unsigned long)&pgt[0],   PDE_GUEST_TABLE);
		pgd[pgd_idx++] = MK_PDE((unsigned long)&pgt[256], PDE_GUEST_TABLE);

		do {
			pgt[pgt_idx++] = MK_PTE(ALLOC_GUEST_PAGE(), PTE_GUEST_PAGE);
		} while((NEXT_TRACK() < dsi->p_end) && (pgt_idx != PGT_ENTRIES_PER_PAGE));

		pgt_idx = 0;

	} while(NEXT_TRACK() < dsi->p_end);

	pgt = (pte_t *)ALLOC_GUEST_TABLE(0);
	pg = ALLOC_GUEST_TABLE(0);

	printf("Src Page = 0x%x\n", GET_HVT_PAGE());

	copy_page(pg, GET_HVT_PAGE());

	pgt[PGT_IDX(VECTORS_VIRT_BASE)] = MK_PTE(pg, PTE_VECTOR_PAGE);

	pgd[PGD_IDX(VECTORS_VIRT_BASE)] = MK_PDE((unsigned long)&pgt[0], PDE_VECTOR_TABLE);

	/* 
	 * For Fixmap Region.
	 * This will be removed as soon as possible.
	 */
	pgd_idx = PGD_IDX(HYPERVISOR_VIRT_START - (1 << PGD_SHIFT) * 2);

	pgt = (pte_t *)ALLOC_GUEST_TABLE(0);
	pgd[pgd_idx++] = MK_PDE((unsigned long)&pgt[0], PDE_GUEST_TABLE);
	pgd[pgd_idx++] = MK_PDE((unsigned long)&pgt[256], PDE_GUEST_TABLE);

#if 0
	install_mapcache_table(v, pgd);
#endif

	zap_low_mappings(pgd);

	/* guest start address (phys/virtual addr) */
	v->arch.guest_pstart = dsi->p_start;
	v->arch.guest_vstart = dsi->v_start;

	/* guest page table address (phys addr) */
	v->arch.guest_table  = mpt_base;
	v->arch.guest_vtable = PGD_ALIGN(dsi->v_end);

	printf("PT FRAMES = %d\n", TOTAL_GUEST_TABLES());
	return TOTAL_GUEST_TABLES();
}

extern void return_to_guest();

void new_thread(struct vcpu *v,
	unsigned long start_pc,
	unsigned long start_stack,
	unsigned long start_info)
{
	void *domain_stack;
	struct cpu_info *ci;
	struct cpu_context *cpu_context;

	domain_stack = pages_m_alloc(STACK_ORDER);
	if(domain_stack == NULL) {
		return;
	}

	ci = (struct cpu_info *)domain_stack;
	ci->vcpu = v;
	ci->vspsr = PSR_MODE_USR;
	ci->vsp = 0;
	ci->vdacr = DOMAIN_KERNEL_VALUE;

	domain_stack += (STACK_SIZE - sizeof(struct cpu_context));

	cpu_context = (struct cpu_context *)domain_stack;
	cpu_context->r0 = 0;
	cpu_context->r12 = start_info;
	cpu_context->usp = start_stack;
	cpu_context->ulr = 0;
	cpu_context->ssp = (unsigned long)(domain_stack + sizeof(struct cpu_context));
	cpu_context->pc = start_pc;
	cpu_context->spsr = PSR_MODE_SVC;

	v->arch.guest_context.user_regs.r13 = (unsigned long)domain_stack;
	v->arch.guest_context.user_regs.r14 = return_to_guest;

	v->arch.guest_context.sys_regs.dacr = DOMAIN_KERNEL_VALUE;
	v->arch.guest_context.sys_regs.cr = get_cr();
}

void new_xen_thread(struct vcpu *v,
	unsigned long start_pc,
	unsigned long context)
{
	void *domain_stack;
	struct cpu_info *ci;

	domain_stack = pages_m_alloc(STACK_ORDER);
	if(domain_stack == NULL) {
		return;
	}

	ci = (struct cpu_info *)domain_stack;
	ci->vcpu = v;
	ci->vspsr = PSR_MODE_SVC;
	ci->vsp = 0;
	ci->vdacr = DOMAIN_HYPERVISOR_VALUE;

	domain_stack += STACK_SIZE;

	v->arch.guest_context.user_regs.r13 = (unsigned long) domain_stack;
	v->arch.guest_context.user_regs.r14 = start_pc;

   /* Fix me. */
	v->arch.guest_context.sys_regs.dacr = 0xdf /* DOMAIN_HYPERVISOR_VALUE */;
	v->arch.guest_context.sys_regs.cr = get_cr();
}

int construct_dom_xen(struct domain *d,
                      xen_domain_fn fn,
                      void *context)
{
   struct vcpu *v;

   BUG_ON(d == NULL);
   BUG_ON(fn == NULL);
   BUG_ON((d->domain_flags & DOMF_xen) == 0);

   v = d->vcpu[0];
   BUG_ON(v == NULL);
   BUG_ON(test_bit(_VCPUF_initialised, &v->vcpu_flags));

   v->arch.guest_table = idle_domain->vcpu[0]->arch.guest_table;

   set_bit(_VCPUF_initialised, &v->vcpu_flags);
   new_xen_thread(v, (unsigned long) fn, context);

   return 0;
}

int construct_dom0(struct domain *d,
		   unsigned long guest_size,
		   unsigned long image_start, unsigned long image_size,
		   unsigned long initrd_start, unsigned long initrd_size,
		   char *cmdline)
{
	char    *p = NULL;
	int     i;
	int     rc;

	unsigned long nr_pages;
	unsigned long nr_pt_pages;

	unsigned long phys_offset;
	unsigned long map_track;

	struct page_info *page = NULL;
	struct start_info *si  = NULL;
	struct domain_setup_info dsi;
	struct vcpu *v         = NULL;

	uint32_t supported_features[XENFEAT_NR_SUBMAPS] = { 0 };
	uint32_t required_features[XENFEAT_NR_SUBMAPS] = { 0 };

	BUG_ON(d == NULL);

	BUG_ON(d->domain_id != 0);
	BUG_ON(d->vcpu[0] == NULL);

	/* Guest partition should be aligned to 1MB boundary */
	ASSERT((guest_size & 0xFFFFF) == 0);

	v = d->vcpu[0];

	BUG_ON(test_bit(_VCPUF_initialised, &v->vcpu_flags));

	memset(&dsi, 0, sizeof(struct domain_setup_info));

	dsi.image_addr = image_start;
	dsi.image_len  = image_size;

	printk("*** LOADING DOMAIN : %d ***\n", (int)d->domain_id);

	d->max_pages = ~0U;

	rc = parseelfimage(&dsi);
	if (rc != 0) {
		return rc;
	}

	if (dsi.xen_section_string == NULL) {
		printk("Not a Xen-ELF image: '__xen_guest' section not found.\n");
		return -EINVAL;
	}

	if ((p = strstr(dsi.xen_section_string, "FEATURES=")) != NULL) {
		parse_features(p + strlen("FEATURES="), supported_features, required_features);

		printk("Guest kernel supports features = { %08x }.\n", supported_features[0]);
		printk("Guest kernel requires features = { %08x }.\n", required_features[0]);

		if (required_features[0]) {
			printk("Guest kernel requires an unsupported hypervisor feature.\n");
			return -EINVAL;
		}
	}

	page = (struct page_info *) pages_u_alloc(d,
                                             get_order_from_bytes(guest_size),
                                             ~ALLOC_DOM_DMA);

	if (page == NULL) {
          printk("Not enough RAM for domain %d allocation (%u pages available)\n",
                 d->domain_id,
                 pages_u_avail());
		return -ENOMEM;
	}

	dsi.p_start = page_to_phys(page);
	dsi.p_end   = dsi.p_start + guest_size;
	printk("Guest physical: 0x%x-0x%x\n", dsi.p_start, dsi.p_end);

	nr_pages = guest_size >> PAGE_SHIFT;

	if (d->tot_pages < nr_pages)
		printk(" (%lu pages to be allocated)", nr_pages - d->tot_pages);

	dsi.v_start &= (~(0xFFFFF));

	nr_pt_pages = build_guest_tables(v, &dsi);
	BUG_ON(nr_pt_pages == 0);
	printk("Guest virtual: 0x%x-0x%x\n", dsi.v_start, dsi.v_end);

	write_ptbase(current);

	BUG_ON(inspect_guest_tables(v) == 0);

	/* Mask all upcalls... */
	for (i = 0; i < MAX_VIRT_CPUS; i++)
		d->shared_info->vcpu_info[i].evtchn_upcall_mask = 1;

	for (i = 1; i < num_online_cpus(); i++)
		(void)alloc_vcpu(d, i, i);

	write_ptbase(v);

	/* Copy the OS image and free temporary buffer. */
	loadelfimage(&dsi);

	map_track = round_pgup((unsigned long)(v->arch.guest_vtable) + (PAGE_SIZE * nr_pt_pages));
	si = (start_info_t *)map_track;
	memset(si, 0, PAGE_SIZE);

	si->nr_pages     = nr_pages;
#if 0
	si->shared_info  = virt_to_phys(d->shared_info);
#endif
	si->shared_info  = d->shared_info;
	si->flags        = SIF_PRIVILEGED | SIF_INITDOMAIN;
	si->pt_base      = (unsigned long)v->arch.guest_vtable;
	si->nr_pt_frames = nr_pt_pages;
	si->mfn_list     = NULL;
	si->min_mfn      = dsi.p_start >> PAGE_SHIFT;

	map_track += PAGE_SIZE;

	if (initrd_size != 0) {

		si->mod_start = map_track;
		si->mod_len = initrd_size;

		printk("Initrd len 0x%lx, start at 0x%lx\n", si->mod_len, si->mod_start);
		memcpy((void *)map_track, (const void *)(initrd_start), initrd_size);
		map_track = round_pgup(map_track + initrd_size);
	}

	memset(si->cmd_line, 0, sizeof(si->cmd_line));
	if (cmdline != NULL)
		strncpy((char *)si->cmd_line, cmdline, sizeof(si->cmd_line)-1);

	write_ptbase(current);

	//init_domain_time(d);

	set_bit(_VCPUF_initialised, &v->vcpu_flags);

	printk("Entry 0x%x\n", dsi.v_kernentry);
	new_thread(v, dsi.v_kernentry, map_track + PAGE_SIZE, (unsigned long)si);

	i = 0;

	BUG_ON(i != 0);

	return 0;
}

int construct_guest_dom(struct domain *d,
			unsigned long guest_size,
			unsigned long image_start, unsigned long image_size,
			unsigned long initrd_start, unsigned long initrd_size,
			char *cmdline)
{
	char    *p = NULL;
	int     i;
	int     rc;

	unsigned long nr_pages;
	unsigned long nr_pt_pages;
	unsigned long map_track;
	unsigned long phys_offset;

	struct page_info *page = NULL; 
	struct start_info *si  = NULL;
	struct domain_setup_info dsi;
	struct vcpu *v         = NULL;

	uint32_t domain_features_supported[XENFEAT_NR_SUBMAPS] = { 0 };
	uint32_t domain_features_required[XENFEAT_NR_SUBMAPS] = { 0 };

	BUG_ON(d == NULL);

	BUG_ON(d->domain_id <= 0);
	BUG_ON(d->vcpu[0] == NULL);

	v = d->vcpu[0];

	printf("Image Start = 0x%x\n", image_start);

	/* Guest partition should be aligned to 1MB boundary */
	ASSERT((guest_size & 0xFFFFF) == 0);

	BUG_ON(test_bit(_VCPUF_initialised, &v->vcpu_flags));

	write_ptbase(idle_domain->vcpu[0]);

	memset(&dsi, 0, sizeof(struct domain_setup_info));

	dsi.image_addr = image_start;
	dsi.image_len  = image_size;

	printk("*** LOADING DOMAIN : %d ***\n", (int)d->domain_id);

	d->max_pages = ~0U;

	rc = parseelfimage(&dsi);
	if (rc != 0) {
        local_irq_enable();
		return rc;
	}


	if (dsi.xen_section_string == NULL) {
		printk("Not a Xen-ELF image: '__xen_guest' section not found.\n");
        local_irq_enable();
		return -EINVAL;
	}

	if ((p = strstr(dsi.xen_section_string, "FEATURES=")) != NULL) {
		parse_features(p + strlen("FEATURES="),
			domain_features_supported,
			domain_features_required);

		printk("Guest kernel supports features = { %08x }.\n",
			domain_features_supported[0]);
		printk("Guest kernel requires features = { %08x }.\n",
                        domain_features_required[0]);

		if (domain_features_required[0]) {
			printk("Guest kernel requires an unsupported hypervisor feature.\n");
			local_irq_enable();
			return -EINVAL;
		}
	}

	page = (struct page_info *) pages_u_alloc(d,
                                             get_order_from_bytes(guest_size),
                                             ~ALLOC_DOM_DMA);
	if (page == NULL) {
		printk("Not enough RAM for domain %d allocation.\n", d->domain_id);
		return -ENOMEM;
	}

	dsi.p_start = page_to_phys(page);
	dsi.p_end   = dsi.p_start + guest_size;
	printk("Guest physical: 0x%x-0x%x\n", dsi.p_start, dsi.p_end);

	dsi.v_start &= (~(0xFFFFF));

	nr_pt_pages = build_guest_tables(v, &dsi);

	write_ptbase(current);

	rc = inspect_guest_tables(v);
	if(!rc) {
		panic("Wrong guest table found\n");
	}

	nr_pages = guest_size >> PAGE_SHIFT;

	if (d->tot_pages < nr_pages)
		printk(" (%lu pages to be allocated)", nr_pages - d->tot_pages);

	for (i = 0; i < MAX_VIRT_CPUS; i++)
		d->shared_info->vcpu_info[i].evtchn_upcall_mask = 1;

	for (i = 1; i < num_online_cpus(); i++)
		(void)alloc_vcpu(d, i, i);

	write_ptbase(v);

	phys_offset = v->arch.guest_pstart - v->arch.guest_vstart;
	dsi.image_addr -= phys_offset;

	/* Copy the OS image and free temporary buffer. */
	(void)loadelfimage(&dsi);

	map_track = round_pgup((unsigned long)(v->arch.guest_vtable) + (PAGE_SIZE * nr_pt_pages));
	si = (start_info_t *)map_track;
	memset(si, 0, PAGE_SIZE);

	si->nr_pages	 = nr_pages;
#if 0
	si->shared_info  = virt_to_phys(d->shared_info);
#endif
	si->shared_info  = d->shared_info;
	si->flags        = 0;
	si->pt_base      = (unsigned long)v->arch.guest_vtable;
	si->nr_pt_frames = nr_pt_pages;
	si->mfn_list     = NULL;
	si->min_mfn      = dsi.p_start >> PAGE_SHIFT;

	map_track += PAGE_SIZE;

	if (initrd_size != 0) {
		si->mod_start = map_track;
		si->mod_len = initrd_size;

		printk("Initrd len 0x%lx, start at 0x%lx\n", si->mod_len, si->mod_start);

		memcpy((void *)map_track, (const void *)(initrd_start - phys_offset), initrd_size);

		map_track = round_pgup(map_track + initrd_size);
	}

	memset(map_track, 0, (PAGE_SIZE * 2));

	si->store_mfn = (map_track + phys_offset) >> PAGE_SHIFT;
	si->store_evtchn = d->store_port;
	
	map_track += PAGE_SIZE;

	si->console_mfn = (map_track + phys_offset) >> PAGE_SHIFT;
	si->console_evtchn = d->console_port;

	map_track += PAGE_SIZE;

	d->console_mfn = si->console_mfn;
	d->store_mfn = si->store_mfn;

	memset(si->cmd_line, 0, sizeof(si->cmd_line));
	if (cmdline != NULL)
		strncpy((char *)si->cmd_line, cmdline, sizeof(si->cmd_line)-1);

#if 0
	/* setup shared info table which is specified each domain */
	rc = setup_shared_info_mapping(d, NULL);

	if (rc != 0) {
		return rc;
	}
#endif
	write_ptbase(current);

	//init_domain_time(d);

	set_bit(_VCPUF_initialised, &v->vcpu_flags);

	new_thread(v, dsi.v_kernentry, map_track + PAGE_SIZE, (unsigned long)si);

	i = 0;

	BUG_ON(i != 0);

	return 0;
}

int elf_sanity_check(Elf_Ehdr *ehdr)
{
	if ( !IS_ELF(*ehdr) ||
		(ehdr->e_ident[EI_DATA] != ELFDATA2LSB) ||
		(ehdr->e_type != ET_EXEC) ) {
		printk("DOM0 image is not a Xen-compatible Elf image.\n");
		return 0;
	}

	return 1;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
