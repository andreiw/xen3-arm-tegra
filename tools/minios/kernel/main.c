#include <os/os.h>
#include <os/irq.h>
#include <os/traps.h>
#include <os/evtchn.h>
#include <os/time.h>
#include <os/sched.h>
#include <os/lib.h>
#include <os/mm.h> 
#include <os/xmalloc.h>
#include <os/hypervisor.h>
#include <os/gnttab.h>

#define SHARED_PTE_MASK	((0xE << 0) | ( 0xFF << 4))
#define __pte(x) (x)

/* Assembler interface fns in entry.S */
void hypervisor_callback(void);
void failsafe_callback(void);

shared_info_t *HYPERVISOR_shared_info;
start_info_t start_info;
unsigned long min_mfn;


extern char shared_info[4096 * 8];
extern void testbench_init();

shared_info_t *map_shared_info(unsigned long pa)
{
	unsigned int offset;

	printk("sizeo fo pte_t = %d\n", sizeof(pte_t));

	if ( (offset = HYPERVISOR_update_va_mapping((unsigned long)shared_info, __pte(pa | SHARED_PTE_MASK), 
					  UVMF_SHARED_INFO | UVMF_INVLPG)) < 0) {
		printk("Failed to map shared_info!!\n");
//        *(int*)0=0;
	}
    
    	printk("Shared info Offset = 0x%x\n", offset);

	return (shared_info_t *)((int)shared_info + (offset << PAGE_SHIFT));
}


void show_start_info(start_info_t *si)
{
	printk("Xen Minimal OS!\n");
        printk("Magic : %s\n", si->magic);
	printk("Total Pages allocated to this domain : %ld\n", si->nr_pages);
	printk("MACHINE address of shared info struct : 0x%x\n", si->shared_info);
	printk("VIRTUAL address of page directory : 0x%x\n", si->pt_base);
	printk("Number of bootstrap p.t. frames : %ld\n", si->nr_pt_frames);
	printk("VIRTUAL address of page-frame list : 0x%x\n", si->mfn_list);
	printk("VIRTUAL address of pre-loaded module : 0x%x\n", si->mod_start);
	printk("Size (bytes) of pre-loaded modules : %ld\n", si->mod_len);
	printk("min mfn(min_page in xen) : %ld\n", si->min_mfn);
	printk("Command-Linux Address : 0x%x\n", si->cmd_line);
	printk("Command-Line String : %s\n", si->cmd_line);
	printk("  flags       : 0x%x\n",  (unsigned int)si->flags);

}

/* execute guest domain from dom0 */
void create_guest_domain()
{
        int ret;
        unsigned long domain_id;
        dom0_op_t dom0_op = { 0 };

        dom0_op.cmd = DOM0_CREATEDOMAIN;
        ret = HYPERVISOR_dom0_op(&dom0_op);
	if (ret < 0)
		printk(" guest domain execution failed!! \n");
        domain_id = dom0_op.u.createdomain.domain;

        dom0_op.cmd = DOM0_GUEST_IMAGE_CTL;
        dom0_op.u.guest_image_info.domain = domain_id;
        dom0_op.u.guest_image_info.sub_cmd = CMD_GUEST_CREATE;
        ret = HYPERVISOR_dom0_op(&dom0_op);
	if (ret < 0)
		printk(" guest domain execution failed!! \n");
	
	dom0_op.cmd = DOM0_UNPAUSEDOMAIN;
	dom0_op.u.unpausedomain.domain = domain_id;
	ret = HYPERVISOR_dom0_op(&dom0_op);
	if (ret < 0)
		printk(" guest domain unpause failed!! \n");
}

int is_current_dom0(void)
{
	if( (unsigned long *)HYPERVISOR_shared_info == (unsigned long *)0xFEBFF000 )
		return 1;
	else
		return 0;
}

void start_kernel(start_info_t *si)
{
	static char banner[] = "Bootstrapping .....  \n";

	printk(banner);
	
	show_start_info(si);
	memcpy(&start_info, (start_info_t *)si, sizeof(start_info_t));

#if 0
	/*
	 * Mapping shared page
	 */
	HYPERVISOR_shared_info = map_shared_info(start_info.shared_info);
#endif

	HYPERVISOR_shared_info = (shared_info_t *)start_info.shared_info;;
	printk("Shared Info : 0x%x\n", (unsigned long)HYPERVISOR_shared_info);

	/* 
	 * Registering callback handler, hypervisor_callback and failsafe_callback
	 */

	HYPERVISOR_set_callbacks((unsigned long)hypervisor_callback, (unsigned long)failsafe_callback);

	if( is_current_dom0() )
		create_guest_domain();

	while(1) 
		printk("[0] dom0\n");
#if 0
	/* init memory management */
	mm_init();

	printk("evtchn_init\n");
	/* init event channl */
	evtchn_init();

	printk("timer_init\n");
	/* init timer */
	time_init();

	printk("sched_init\n");
	sched_init();


	printk("fork dom1\n");

//	local_irq_enable();
	run_idle_thread();
#endif

}
