/*
 * acm_hooks.c 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          BokDeuk Jeong  <bd.jeong@samsung.com>
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

/*
 * acm_hooks.c implements access control hooks.
 * Those hooks are usually called inside hypercalls.
 * Each hook generates request information and asks ACM integrator for access control decisionis.
 * Hook functions return 1 as grant or 0 as denial.
*/
#include <security/acm/acm.h>
#include <security/acm/acm_hooks.h>
#include <security/acm/aci_generator.h>
#include <public/security/secure_storage_struct.h>
#include <asm/current.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <public/acm_ops.h>
#include <public/acm_dev.h>
#include <xen/rangeset.h>
#include <xen/event.h>


inline int acm_domain_simple_op(struct domain *dom, uint32_t req_type)
{
	aci_domain subject, object_dom;
	aci_object object;
	aci_context context;

#ifndef CONFIG_SUBSTITUTE_IS_PRIV
	return 1;
#else
	if(dom == current->domain)
		return 1;

	get_domain_info(current->domain, &subject);
	object.object_type = ACM_DOMAIN;
	get_domain_info(dom, &object_dom);
	object.object_info = &object_dom;
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, req_type, &context);
#endif	
}	

int acm_pause_domain(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_PAUSE);
}

int acm_unpause_domain(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_UNPAUSE);
}

int acm_create_domain(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_CREATE);
}

int acm_copy_domain_image(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_COPY_IMAGE);
}

int acm_destroy_domain(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_DESTROY);
}

int acm_remote_shutdown(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_SHUTDOWN);
}

int acm_get_domaininfo(struct domain *dom)
{
	return acm_domain_simple_op(dom, DOM_GET_INFO);
}

inline int acm_domain_control_op(struct domain *dom, uint32_t obj_type, uint32_t req_type)
{
	if(current->domain->domain_id == IDLE_DOMAIN_ID)
		return 1;

#ifndef CONFIG_SUBSTITUTE_IS_PRIV
	return 1;
#else
	{
		aci_domain subject, object_dom;
		aci_object object;
		aci_context context;

		get_domain_info(current->domain, &subject);
		object.object_type = obj_type;
		object.struct_type = OBJST_TYPE_ONDOMAIN;
		get_domain_info(dom, &object_dom);
		object.object_info = &object_dom;
		get_environmental_context(&context);

		return acm_is_granted(&subject, &object, req_type, &context);
	}
#endif
}


static int acm_vcpu_common_op(struct domain *dom, struct vcpu* v, uint32_t req_type)
{

	aci_domain subject;
	aci_object object;
	struct aci_vcpu vcpu;
	aci_context context;

	if(current->domain != dom){
		if(!acm_domain_control_op(dom, ACM_VCPU, VCPU_CONTROL))
			return 0;
	}

	get_domain_info(current->domain, &subject);
	object.object_type = ACM_VCPU;
	object.object_info = &vcpu;
	vcpu.vcpu = v;
	get_vcpu_info(dom, &vcpu);
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, req_type, &context);
}

int acm_set_vcpucontext(struct domain *dom, struct vcpu *v)
{
	return acm_vcpu_common_op(dom, v, VCPU_SET_CONTEXT);
}

int acm_get_vcpucontext(struct domain *dom, struct vcpu *v)
{
	return acm_vcpu_common_op(dom, v, VCPU_GET_CONTEXT);
}

int acm_get_vcpuinfo(struct domain *dom, struct vcpu *v)
{
	return acm_vcpu_common_op(dom, v, VCPU_GET_INFO);
}

int acm_set_maxvcpus(struct domain *dom)
{
	return acm_vcpu_common_op(dom, NULL, VCPU_SET_MAX);
}

int acm_set_vcpuaffinity(struct domain *dom, struct vcpu *v)
{
	return acm_vcpu_common_op(dom, v, VCPU_SET_AFFINITY);
}

int acm_set_domainmaxmem(struct domain *dom)
{
	if(current->domain != dom){
		if(!acm_domain_control_op(dom, ACM_MEMORY, MEM_CONTROL))
			return 0;
	}

	return 1;
}

int acm_set_domainhandle(struct domain *dom)
{
	if(current->domain != dom){
		return acm_domain_control_op(dom, ACM_HANDLE, HANDLE_SET);
	}else
		return 1;
}

int acm_set_debugging(struct domain *dom)
{
	if(current->domain == dom)
		return 1;
	return acm_domain_control_op(dom, ACM_DEBUG, DEBUG_SET);
}

int acm_irq_permission(struct domain *dom, uint8_t pirq, int nr_pirq)
{
	aci_domain subject;
	aci_object object;
	struct aci_pirq irq;
	aci_context context;
	int i, end;
	int result = 0;

	if(nr_pirq <= 0)
		return 1;

	if(current->domain != dom){
		if(!acm_domain_control_op(dom, ACM_PIRQ, PIRQ_CONTROL))
			return 0;
	}

	get_environmental_context(&context);
	get_domain_info(dom, &subject);
	object.object_type = ACM_PIRQ;

	ASSERT(pirq < NR_IRQS);

	end = pirq + nr_pirq;

	for(i=pirq; i < end; i++){
		object.object_info = &irq;
		get_pirq_info(i, &irq);
		if(acm_is_granted(&subject, &object, PIRQ_ACCESS, &context))
			result++;
		else
			break;
	}

	return result;
}

int acm_irq_permit_access(struct domain *dom, unsigned int start_pirq, unsigned int end_pirq)
{
	int start, end, nr_remain, nr_grant;
	int rtn = 0;

	start = start_pirq;
	end = end_pirq + 1;
	nr_remain = end - start;

	while(nr_remain > 0){
		nr_grant = acm_irq_permission(dom, start, nr_remain);

		if(nr_grant > 0){
			rtn = rangeset_add_range(dom->irq_caps, start, start + nr_grant -1);

			if(rtn < 0)
				return rtn;

			start = start + nr_grant + 1;
			nr_remain = end - start;
		}else{
			start++;
			nr_remain--;
		}
	}

	return rtn;
}

int acm_irq_access_permitted(struct domain *dom, unsigned int start_pirq, unsigned int end_pirq)
{
	int result;

	if(rangeset_contains_range(dom->irq_caps, start_pirq, end_pirq))
			result = 1;
	else{
		acm_irq_permit_access(dom, start_pirq, end_pirq);
		result = rangeset_contains_range(dom->irq_caps, start_pirq, end_pirq);
	}

	return result;
}

int acm_send_guest_pirq(struct domain *dom, uint8_t pirq)
{
	aci_domain subject;
	aci_object object;
	struct aci_pirq irq;
	aci_context context;

	get_domain_info(dom, &subject);
	object.object_type = ACM_PIRQ;
	object.object_info = &irq;

	ASSERT(pirq < NR_IRQS);

	get_pirq_info(pirq, &irq);
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, PIRQ_ACCESS, &context);
}

int acm_set_HID_irq(uint32_t pirq)
{
	aci_domain subject;
	aci_object object;
	struct aci_pirq irq;
	aci_context context;

	get_domain_info(current->domain, &subject);
	object.object_type = ACM_PIRQ;
	object.object_info = &irq;

	ASSERT(pirq < NR_IRQS);

	get_pirq_info(pirq, &irq);
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, PIRQ_SET_HID, &context);
}

int acm_irq_status_query(uint32_t pirq)
{
	aci_domain subject;
	aci_object object;
	struct aci_pirq irq;
	aci_context context;

	get_domain_info(current->domain, &subject);
	object.object_type = ACM_PIRQ;
	object.object_info = &irq;

	ASSERT(pirq < NR_IRQS);

	get_pirq_info(pirq, &irq);
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, PIRQ_QUERY_STAT, &context);
}

int acm_set_irq_type(void)
{
	return acm_resource_simple_op(ACM_PIRQ, PIRQ_SET_TYPE);
}

/* Returns the number of granted mfns until the rejection */
static int acm_iomem_permission(struct domain *dom, unsigned long start_paddr, unsigned long end_paddr)
{
	aci_domain subject;
	aci_object object;
	aci_context context;
	struct aci_iomem iomem;
	unsigned long addr;
	unsigned long unit_size;

	int result = 0;

	/* TEMPORILY BLOCKED UNTIL PAGING MECHANISM IMPLEMENTATION IS DONE */
	return 1;

	if(end_paddr <= start_paddr)
		return 1;

	if(current->domain != dom){
		if(!acm_domain_control_op(dom, ACM_IOMEM, IOMEM_CONTROL))
			return 0;
	}

	get_environmental_context(&context);
	get_domain_info(dom, &subject);
	object.object_type = ACM_IOMEM;
	object.object_info = &iomem;
	unit_size = get_dev_unit_size(start_paddr);

	for(addr = start_paddr; addr <= end_paddr; addr += unit_size){
		get_iomem_info(addr, object.object_info);

		if(acm_is_granted(&subject, &object, IOMEM_ACCESS, &context))
			result++;
		else
			break;
	}

	return result;
}

int acm_iomem_permit_access(struct domain *dom, unsigned long first_mfn, unsigned long end_mfn)
{
	int start, end, nr_remain, nr_grant;
	int rtn;
	
	rtn = 0;

	start = first_mfn;
	end = end_mfn + 1;
	nr_remain = end - start;

	while(nr_remain > 0){
		nr_grant = acm_iomem_permission(dom, start << PAGE_SHIFT, ((start + nr_remain) << PAGE_SHIFT) -1);

		if(nr_grant > 0){
			rtn = rangeset_add_range(dom->iomem_caps, start, start + nr_grant -1);

			if(rtn < 0)
				return rtn;

			start = start + nr_grant + 1;
			nr_remain = end - start;
		}else{
			start++;
			nr_remain--;
		}
	}

	return rtn;
}

int acm_iomem_access_permitted(struct domain *dom, unsigned int start_mfn, unsigned int end_mfn)
{
	int result;

	if(rangeset_contains_range(dom->iomem_caps, start_mfn, end_mfn))
			result = 1;

	else{
		acm_iomem_permit_access(dom, start_mfn, end_mfn);
		result = rangeset_contains_range(dom->iomem_caps, start_mfn, end_mfn);
	}

	return result;
}

inline int acm_resource_simple_op(uint32_t obj_type, uint32_t req_type) 
{
	aci_domain subject;
	aci_object object;
	aci_context context;
	get_domain_info(current->domain, &subject);
	object.object_type = obj_type;
	object.object_info = NULL;
	object.struct_type = OBJST_TYPE_SIMPLE;
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, req_type, &context);
}

int acm_sched_ctl(void)
{
	return acm_resource_simple_op(ACM_SCHEDULER, SCHED_SET_CTX_SWITCH_TIME);
}

int acm_adjust_dom(void)
{
	return acm_resource_simple_op(ACM_SCHEDULER, SCHED_ADJUST);
}

int acm_sched_get_id(void)
{
	return acm_resource_simple_op(ACM_SCHEDULER, SCHED_GET_ID);
}

int acm_settime(void)
{
	return acm_resource_simple_op(ACM_TIME, TIME_SET);
}

/* TODO: DOM0_READCONSOLE is currently not implemented in do_dom0_op()! */
int acm_readconsole(uint32_t clear)
{
	return 1;
}

int acm_tbuf_control(void)
{
	return acm_resource_simple_op(ACM_TRACEBUFFER, TRACE_CONTROL);
}

int acm_perfc_control(void)
{
	return acm_resource_simple_op(ACM_PERFCOUNTER, PERFC_CONTROL);
}

static inline int acm_evtchn_communicate(struct domain *ldom, 
					 struct domain *rdom, 
					 evtchn_port_t port, 
					 uint32_t use, 
					 uint32_t req_type)
{
	aci_domain subject;
	aci_object object;
	aci_context context;
	struct aci_evtchn evtchn;

	if(ldom == rdom)
		return 1;

	if(req_type == EVTCHN_OPEN){
		if(current->domain != ldom && !acm_domain_control_op(ldom, ACM_EVENTCHN, EVTCHN_CONTROL))
			return 0;
		get_evtchn_info(ldom, rdom, port, use, &evtchn);
	}

	get_domain_info(ldom, &subject);
	object.object_type = ACM_EVENTCHN;
	object.object_info = &evtchn;
	get_domain_info(rdom, &evtchn.objdom_id);
	evtchn.use = use;
	evtchn.port = port;
	get_environmental_context(&context);

	return acm_is_granted(&subject, &object, req_type, &context);
}

int acm_evtchn_alloc_unbound(struct domain *dom, domid_t rdomid, evtchn_port_t port, uint32_t use)
{
	struct domain *rdom = NULL;
	int result;

	if(rdomid == DOMID_SELF)
		rdom = current->domain;
	else
		rdom = find_domain_by_id(rdomid);
	if(!rdom)
		return 1;

	result = acm_evtchn_communicate(dom, rdom, port, use, EVTCHN_OPEN);
	put_domain(rdom);

	return result;
}

int acm_evtchn_bind_interdomain(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use)
{
	return acm_evtchn_communicate(dom, rdom, port, use, EVTCHN_OPEN);
}


int acm_evtchn_send(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use)
{
	return acm_evtchn_communicate(dom, rdom, port, use, EVTCHN_SEND);
}

/* AS LONG AS 'irq_access_permitted' is used correctly, 
 * we don't need access control to the function below. 
 */
int acm_evtchn_bindpirq(uint32_t pirq)
{
	return acm_irq_permission(current->domain, pirq, 1);
}

int acm_evtchn_status(struct domain *dom, evtchn_port_t port, uint32_t use)
{
	if(dom == current->domain)
		return 1;

	return acm_domain_control_op(dom, ACM_EVENTCHN, EVTCHN_GET_STATUS);
}

extern struct vcpu *drvdom_vcpu;

int acm_evtchn_bindvirq(uint32_t vcpu, uint32_t virq_num)
{
	aci_domain subject;
	aci_object object;
	aci_context context;
	struct aci_virq virq;
	int result;

	get_domain_info(current->domain, &subject);
	object.object_type = ACM_VIRQ;
	object.object_info = &virq;
	virq.vcpu_id = vcpu;
	virq.virq = virq_num;

	get_environmental_context(&context);
	result = acm_is_granted(&subject, &object, VIRQ_BIND, &context);

	if(result && virq_num == VIRQ_DDOM_POLICY)
		drvdom_vcpu = current->domain->vcpu[vcpu];	

	return result;
}

int acm_evtchn_virq_status(struct domain *dom, evtchn_port_t port, uint32_t virq_num)
{
	aci_domain subject;
	aci_object object;
	aci_context context;
	struct aci_virq virq;
	int result;
	struct vcpu *v = NULL;

	get_domain_info(current->domain, &subject);

	for_each_vcpu(dom, v){
		if(v->virq_to_evtchn[virq_num] == port){
			virq.vcpu_id = v->vcpu_id;
			break;
		}
	}

	virq.virq = virq_num;
	get_environmental_context(&context);

	result = acm_is_granted(&subject, &object, VIRQ_GET_STATUS, &context);
	
	return result;
}

int acm_granttable_op(struct domain *subjdom, struct domain *objdom, uint32_t mfn, uint32_t use, uint32_t req_type)
{
	aci_domain subject;
	aci_object object;
	aci_context context;
	struct aci_gnttab gnttab;
	int result;
	
	get_domain_info(subjdom, &subject);
	object.object_type = ACM_GRANTTAB;
	object.object_info = &gnttab;
	gnttab.mfn = mfn;
	gnttab.use = use;
	get_granttab_info(objdom, mfn, &gnttab);
	get_environmental_context(&context);

	result = acm_is_granted(&subject, &object, req_type, &context);
	
	return result;
}

int acm_granttable_share(struct domain *subjdom, struct domain *objdom, uint32_t mfn, uint32_t use, uint32_t flags)
{
	//printk("[acm_granttable_share] subj:%d obj:%d use:0x%x \n", subjdom->domain_id, objdom->domain_id, use);
	if(flags| GNTMAP_readonly)
		return acm_granttable_op(subjdom, objdom, mfn, use, GNTTAB_READ);
	else
		return acm_granttable_op(subjdom, objdom, mfn, use, GNTTAB_READ || GNTTAB_WRITE);
}

int acm_granttable_transfer(struct domain *subjdom, struct domain *objdom, uint32_t mfn, uint32_t use)
{
	//printk("[acm_granttable_transfer] subj:%d obj:%d use:0x%x \n", subjdom->domain_id, objdom->domain_id, use);
	return acm_granttable_op(subjdom, objdom, mfn, use, GNTTAB_TRANSFER);
}

int acm_granttable_setup(struct domain *dom)
{

	if(dom == current->domain)
		return 1;

	return acm_domain_control_op(dom, ACM_GRANTTAB, GNTTAB_SETUP);
}

int acm_granttable_dumptab(struct domain *dom)
{

	if(dom == current->domain)
		return 1;

	return acm_domain_control_op(dom, ACM_GRANTTAB, GNTTAB_DUMP_TABLE);
}


/* Controls privileges to reserving memory space for other domains
 * do_memory_op is the only way to increase or decrease memory space allocated to a domain
 * after dom creation.
 * Domain's tot_pages = xenheap_pages +(-) # of transferred page via grant_table
 */
inline int acm_alloc_domheap(struct domain *dom, unsigned int pages)
{

	aci_domain subject;
	aci_object object;
	aci_context context;
	struct aci_memory mem_heap;
	int result = 0;

	if(current->domain != dom){
		if(!acm_domain_control_op(dom, ACM_MEMORY, MEM_CONTROL))
			return 0;
	}

	get_domain_info(dom, &subject);
	object.object_type = ACM_MEMORY;
	object.object_info = &mem_heap;
	mem_heap.req_pages = pages;
	get_memory_info(dom, &mem_heap);
	get_environmental_context(&context);

	result = acm_is_granted(&subject, &object, MEM_ALLOC_DOMHEAP, &context);

	return result;
}


int acm_increase_reservation(struct domain *dom, unsigned int extent_order)
{
	return acm_alloc_domheap(dom, 1UL << extent_order);
}

int acm_decrease_reservation(struct domain *dom)
{
	/* We don't care that a domain downsizes its memory */
	if(dom == current->domain)
		return 1;

	return acm_domain_control_op(dom, ACM_MEMORY, MEM_CONTROL);
}

int acm_populate_physmap(struct domain *dom, unsigned int extent_order)
{
	return acm_alloc_domheap(dom, 1UL << extent_order);
}

int acm_current_reservation(struct domain *dom)
{
	if(dom == current->domain)
		return 1;

	return acm_domain_control_op(dom, ACM_MEMORY, MEM_GET_STAT);
}

int acm_translate_gpfn_list(struct domain *dom)
{
	if(dom == current->domain)
		return 1;

	return acm_domain_control_op(dom, ACM_MEMORY, MEM_TRANSLATE_ADDR);
}

// controls how much memory space a domain can take.
int acm_alloc_chunk(struct domain *dom, unsigned int order)
{
	return acm_alloc_domheap(dom, 1UL << order);
}

int acm_set_guest_pages(struct domain *dom, unsigned int size)
{
	return acm_alloc_domheap(dom, size >> PAGE_SHIFT);
}

/*refer public/xen.h */
int acm_modify_pte(pte_t nl1e)
{
	int result;
	int page_size;

	if(acm_belong_to_iomem((unsigned long)nl1e.l1)){
		if(((unsigned long)nl1e.l1 & PTE_TYPE_MASK) == PTE_TYPE_SMALL)
			page_size = PAGE_SIZE;				// 4KB
		else if(((unsigned long)nl1e.l1 & PTE_TYPE_MASK) == PTE_TYPE_LARGE)
			page_size = PAGE_SIZE << 4;		// 64KB
		else if(((unsigned long)nl1e.l1 & PTE_TYPE_MASK) == PTE_TYPE_TINY)
			page_size = PAGE_SIZE >> 2;		// 1KB
		else{
			page_size = PAGE_SIZE;
			printk("[acm_modify_pte] mapping size unknown for 0x%x\n", (unsigned long)nl1e.l1);
		}
		//printk("[acm_modify_pte] mapping size: 0x%x\n", page_size);
		result = acm_iomem_permission(current->domain, nl1e.l1, nl1e.l1 + page_size - 1);
	}else
		result = 1;
		
	return result;
}

int acm_modify_pde(pde_t nl2e)
{
	int result;
	
	if(acm_belong_to_iomem((unsigned long)nl2e.l2) && 
		(((unsigned long)nl2e.l2 & PDE_TYPE_MASK) == PDE_TYPE_SECTION) )
		result = acm_iomem_permission(current->domain, nl2e.l2, nl2e.l2 + SECTION_SIZE - 1);
	else
		result = 1;

	return result;
}

int acm_mod_default_entry(unsigned long paddr)
{
	int result;
	int page_size;

	result = 1;

	if(acm_belong_to_iomem(paddr)){
		if((paddr & PTE_TYPE_MASK) == PTE_TYPE_SMALL)
			page_size = PAGE_SIZE;
		else if((paddr & PTE_TYPE_MASK) == PTE_TYPE_LARGE)
			page_size = PAGE_SIZE << 4;
		else if((paddr & PTE_TYPE_MASK) == PTE_TYPE_TINY)
			page_size = PAGE_SIZE >> 2;
		else{
			page_size = PAGE_SIZE;
			//printk("[acm_mod_default_entry] mapping size unknown for 0x%x\n", paddr);
		}
		//printk("[acm_mod_default_entry] mapping size: 0x%x\n", page_size);

		//result = acm_iomem_permission(current->domain, paddr, paddr + DEV_UNIT_ADDR_SIZE - 1);
		result = acm_iomem_permission(current->domain, paddr, paddr + page_size - 1);
		
	}else
		result = 1;

	return result;
}

int acm_mmu_update(unsigned long mfn)
{
	return 1;
}

int acm_mmuext_op(void)
{
		return 1;
}

int acm_update_va_mapping(struct domain *dom)
{
	return 1;
}

int acm_console_io(int cmd)
{
	uint32_t req_type = 0;

	switch(cmd){
		case CONSOLEIO_write:
			req_type  = CONSOLE_WRITE;
			break;

		case CONSOLEIO_read:
			req_type = CONSOLE_READ;
			break;
	}
			
	return acm_resource_simple_op(ACM_CONSOLEIO, req_type);
}

int acm_set_foregrounddom(void)
{
	return acm_resource_simple_op(ACM_ETC, ETC_SET_FOREGROUNDDOM);
}

int acm_access_policy(int flag)
{

	uint32_t req_type = 0;

#ifndef CONFIG_SUBSTITUTE_IS_PRIV
	return 1;
#else
	switch(flag){
		case VMM:
			req_type = POLICY_ACCESS_VMM;
			break;

		case DRVDOM:
			req_type = POLICY_ACCESS_DDOM;
			break;
	}
#endif
	return acm_resource_simple_op(ACM_POLICY, req_type);
}

int acm_sra_read(partition_type_t part)
{
	uint32_t req_type;
	switch(part){
	case PART_SP3:	
		req_type = SECUSTR_READ_SP3;
		break;
	case PART_OS_IMAGE:	
		req_type = SECUSTR_READ_OSIMG;
		break;
	default:
		req_type = -1;
	}
	if(req_type != -1)
		return acm_resource_simple_op(ACM_SECUSTORAGE, req_type);
	else
		return 1;
}

int acm_sra_read_enc_part(partition_type_t part)
{
	uint32_t req_type;
	switch(part){
	case PART_MBB:
		req_type = SECUSTR_READ_ENCMBB;
		break;
	case PART_SP1:	
		req_type = SECUSTR_READ_ENCSP1;
		break;
	case PART_SP2:	
		req_type = SECUSTR_READ_ENCSP2;
		break;
	case PART_SP3:	
		req_type = SECUSTR_READ_ENCSP3;
		break;
	case PART_OS_IMAGE:
		req_type = SECUSTR_READ_ENCOSIMG;
		break;
	default:
		req_type = -1;
	}
	if(req_type != -1)
		return acm_resource_simple_op(ACM_SECUSTORAGE, req_type);
	else
		return 1;
}

int acm_sra_write(partition_type_t part, image_type_t type)
{
	uint32_t req_type;
	switch(part){
	case PART_SP1:
		switch(type){
		case XEN_ARM_SIGNED_HASH:
			req_type = SECUSTR_WRITE_VMMSIG;
			break;
		case CERTM_SIGNED_HASH:
			req_type = SECUSTR_WRITE_CERT;
			break;
		case DRIVER_DOM_SIGNED_HASH:
		case SECURE_DOM_SIGNED_HASH:
		case NORMAL_DOM1_SIGNED_HASH:
		case NORMAL_DOM2_SIGNED_HASH:
		case NORMAL_DOM3_SIGNED_HASH:
		case NORMAL_DOM4_SIGNED_HASH:
			req_type = SECUSTR_WRITE_OSSIG;
			break;
		default:
			req_type = -1;
		}
		break;
	case PART_SP3:	
		req_type = SECUSTR_WRITE_SP3;
		break;
	case PART_OS_IMAGE:
		req_type = SECUSTR_WRITE_OSIMG;
		break;
	default:
		req_type = -1;
	}
	if(req_type != -1)
		return acm_resource_simple_op(ACM_SECUSTORAGE, req_type);
	else
		return 1;
}


/* hooks for resource state update  */

void acm_weight_dom_cpu(struct domain *d)
{
	ASSERT(d);

	if(d->domain_id < DOMID_FIRST_RESERVED)
		acm_control_cpu_usage(d);
}

/* Check if current domain should be re-weighted or shutdown by ACM battery usage policy.
 * Also checks if the sheduling is is battery saving mode by ACM policy.
 * Return Values:
 * 0 - is not in battery saving mode.
 * 1 - is in battery saving mode.
 */
int nr_btsav_dom = 0;

int acm_check_battery_saving(struct domain *dom)
{
	int decision;
	unsigned int batterylife;

	ASSERT(dom);

	if(dom->domain_id >= DOMID_FIRST_RESERVED)
		return 0;

	batterylife = dom0->shared_info->batterylife;

	/* TEMPORARY FOR TEST */
	if(batterylife == 0)
		dom0->shared_info->batterylife = 100;

	/* To avoid ACM overhead, operate battery management is done at 
	 * every 5 percents.
	 *  *********** TEMPORARILY BLOCKED ********************
	 *  For now, we are using the fake battery measurement. 
	 *  To make it work, we omit batterylife%5==0 condition.
	 *  However, if using a real battery measurement device, 
	 *  the commented line below should be used instead.
	 */
	if(batterylife == dom->acm_batterylife){
		dom->acm_batterylife = batterylife;
		goto MODE_RTN;
	}

	decision = acm_control_battery_usage(batterylife, dom);

	if(decision & ACM_DECISION_SHUTDOWN){
		/* Shutdown_domain is set to 0x0 is regraded as 
		 * dom0 has handled previous shutdown virq. 
		 * If not, it means dom0 hasn't handled previous shutdown virq yet.
		 */
		if(dom0->shared_info->shutdown_domain == 0 && dom->acm_shutdown == 0){
			dom0->shared_info->shutdown_domain = dom->domain_id;
			send_guest_virq(dom0->vcpu[0], VIRQ_BATTERY_CONTROL);
			dom->acm_shutdown = 1;
		}
	}

	dom->acm_batterylife = batterylife;

MODE_RTN:
		if(nr_btsav_dom > 0)
			return 1;
		else
			return 0;
}
