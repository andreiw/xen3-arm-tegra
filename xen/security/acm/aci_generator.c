/*
 * aci_generator.c 
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

#include <xen/config.h>
#include <security/acm/aci_generator.h>
#include <public/acm_dev.h>
#include <asm-arm/page.h>
#include <asm/arch/irqs.h>

/*
 * ACI generator keeps counting the number of communication connections per purpose.
 * Also it has a domain mapping table between a domid and the its hash value.
 */
/* event channl statistics of a domain */
typedef struct evtchn_stat{
	uint32_t evtchn_sum;
	struct rdom_evtchn_stat *list_head;
}evtchn_stat_t;

/* 
 * statistics of event channels per remote dom in a domain
 */
typedef struct rdom_evtchn_stat{
	uint32_t evtchn_count;
	domid_t rdom;
	struct	evtchn_use_stat *list_head;
	struct rdom_evtchn_stat *prev;
	struct rdom_evtchn_stat *next;
}rdom_evtchn_stat_t;

typedef struct evtchn_use_stat{
	uint32_t use;
	uint8_t use_count;
	struct evtchn_use_stat *prev;
	struct evtchn_use_stat *next;
}evtchn_use_stat_t;

#define NR_EVTSTAT_BUCKET	256
#define NR_EVTSTAT_ENTRY_PER_BUCKET	(0x1UL << (sizeof(domid_t)-1)*8)
#define EVTSTAT_BUCKET_SIZE	sizeof(evtchn_stat_t)*NR_EVTSTAT_ENTRY_PER_BUCKET

static evtchn_stat_t *evtchn_usage[NR_EVTSTAT_BUCKET];


void get_domain_info(struct domain *dom, aci_domain *dom_id)
{
	ASSERT(dom);
	ASSERT(dom_id);

	dom_id->id = dom->scid;
}

/*
 * For now, no evironmental context is necessary.
 */
void get_environmental_context(aci_context *context)
{

}

void get_pirq_info(uint8_t pirq_num, struct aci_pirq *pirq)
{
	ASSERT(pirq);
	ASSERT(pirq_num < NR_IRQS);

#if defined(CONFIG_MACHINE_IMX21)
	if(pirq_num > INT_DMACH0 && pirq_num <= INT_DMACH15) 
		pirq->device = INT_DMACH0;
	else if(pirq_num > INT_EMMADEC && pirq_num <= INT_EMMAPP)
		pirq->device = INT_EMMAENC;
	else if(pirq_num > INT_USBMNP && pirq_num <= INT_USBCTRL)
		pirq->device = INT_USBWKUP;
	else 
		pirq->device = pirq_num;
#else
	pirq->device = pirq_num;
#endif
}

#ifdef CONFIG_MACHINE_IMX21
#include <asm/arch-imx21/hardware.h>
#endif
void  get_iomem_info(unsigned long paddr, struct aci_iomem *iomem)
{
	unsigned int dev_unit;
	ASSERT(iomem);	

	iomem->device = UNRSV;

#if defined(CONFIG_MACHINE_IMX21)
	if((paddr >= 0xC8000000) && (paddr < 0xCC000000))
		iomem->device = IMX21_FLASH_MEM;
	else if((paddr >= 0xCC000000) && (paddr < 0xD0000000))
		iomem->device = IMX21_CS1;
	else if((paddr >= 0x10000000) && (paddr < 0x10040000))
		iomem->device = (paddr - IMX_IO_PHYS) >> DEV_UNIT_ADDR_SHIFT;
	else if((paddr >= 0xDF000000) && (paddr < 0xDF002000))
		iomem->device = IMX21_CHIPSELECT;
	else if((paddr >= 0xDF002000) && (paddr < 0xDF003000))
		iomem->device = IMX21_PCMCIA;
	else if((paddr >= 0xDF003000) && (paddr < 0xDF004000))
		iomem->device = IMX21_NANDFC;
#elif defined(CONFIG_MACHINE_XSCALE)
	dev_unit = paddr >> DEV_UNIT_ADDR_SHIFT;
	if(dev_unit <= ((PXA_CS1_PADDR + PXA_CS1_SIZE) >> DEV_UNIT_ADDR_SHIFT)){
		/* Chip Selects */
		if(paddr >= PXA_CS1_PADDR)
			iomem->device = PXA_CS1;
		else if(paddr >= PXA_PCMCIA_PADDR)
			iomem->device = PXA_PCMCIA;
		else if(paddr >= PXA_CS3_PADDR)
			iomem->device = PXA_CS3;
		else
			iomem->device = PXA_CS0;
	}else if(dev_unit < (0x80000000 >> DEV_UNIT_ADDR_SHIFT)){
		/* IO registers */
		if(dev_unit == 0x460){
			if(paddr >= MIRAGE_ISBA_PMD_PADDR)
				iomem->device = MIRAGE_ISBA_PMD;
			else
				iomem->device = MIRAGE_MLCD;
		}else
			iomem->device = paddr >> DEV_UNIT_ADDR_SHIFT;
	}else{
		/* DDR Chip selects */
		if(paddr < PXA_DDR_CS1_PADDR)
			iomem->device = PXA_DDR_CS0;
		else
			iomem->device = PXA_DDR_CS1;
	}
#endif	
}

unsigned long get_dev_unit_size(unsigned long paddr)
{	
#if defined(CONFIG_MACHINE_IMX21)
	if((paddr >= 0xC8000000) && (paddr < 0xD0000000))
		return PAGE_SIZE;
	else
		return DEV_UNIT_ADDR_SIZE;
#endif		
		return DEV_UNIT_ADDR_SIZE;
}

void get_vcpu_info(struct domain *obj_dom, struct aci_vcpu *vcpu)
{
	ASSERT(vcpu);

	get_domain_info(obj_dom, &vcpu->objdom_id);
	if(vcpu->vcpu != NULL)
		vcpu->vcpu_id = vcpu->vcpu->vcpu_id;
	else
		vcpu->vcpu_id = 0;
}

inline void get_evtchn_info(struct domain *dom, struct domain *rdom, evtchn_port_t port, uint32_t use, struct aci_evtchn *evtchn)
{
	ASSERT(dom);
	ASSERT(rdom);
	ASSERT(evtchn);

	get_evtchn_stat(dom->domain_id, rdom->domain_id, use, &evtchn->tot_evtchn, &evtchn->per_rdom, &evtchn->per_rdom_use);
}

/* TODO: identify mem space which the mfn belongs to */
void get_granttab_info(struct domain *dom, uint32_t mfn, struct aci_gnttab *gnttab)
{
	ASSERT(gnttab);

	get_domain_info(dom, &gnttab->objdom_id);

	if(acm_belong_to_iomem(mfn << PAGE_SHIFT))
		gnttab->mem_space = MEM_IO_SPACE;
	else
		gnttab->mem_space = 0;
		
	return; 
}

/*
 * In struct domain,
 *	tot_pages;     <--- # of xenheap pages +(-) # of shared pages
 *	xenheap_pages; <--- # of RAM pages assigned to a dom 
 */
extern unsigned long total_pages;

void get_memory_info(struct domain *d, struct aci_memory *mem)
{
	ASSERT(d);
	ASSERT(mem);	

	mem->xenheap_pages = d->xenheap_pages;
	mem->sys_total     = total_pages;

	printk("ACM: total_pages: %lu\n", total_pages);
}

void update_resource_state(void)
{
	return;	
}

#define PAGE_FAULT_MASK 0x3UL
int acm_belong_to_iomem(unsigned long paddr)
{
	if(!(paddr & PAGE_FAULT_MASK))
		return 0;

#if defined(CONFIG_MACHINE_IMX21)
	if( ((paddr >= 0x10000000)&&(paddr <0x20000000)) ||
		  ((paddr >= 0xC8000000)&&(paddr < 0xD0000000)) ||
		  (paddr >= 0xDF000000) )
        	return 1;
#elif defined(CONFIG_MACHINE_XSCALE)
	if((paddr >= 0x0) && (paddr < 0x60000000))
		return 1;
#endif
	return 0;
}


void create_evtchn_stat(domid_t domid)
{
	int bucket_num = domid/NR_EVTSTAT_ENTRY_PER_BUCKET;

	if(!evtchn_usage[bucket_num]){

		evtchn_stat_t *new_evtchn_stat = NULL;
		new_evtchn_stat = xmalloc_array(evtchn_stat_t, NR_EVTSTAT_ENTRY_PER_BUCKET);

		ASSERT(new_evtchn_stat);

		if(!new_evtchn_stat){
			printk("EVTCHN_STAT: Unable to allocate memory for event channel stat\n");
			return;
		}

		evtchn_usage[bucket_num] = new_evtchn_stat;
		memset(new_evtchn_stat, 0, EVTSTAT_BUCKET_SIZE);
	}
}

void destroy_evtchn_stat(domid_t domid)
{
	evtchn_stat_t *dom_stat       = NULL;
	rdom_evtchn_stat_t *rdom_stat = NULL;
	evtchn_use_stat_t *use_stat   = NULL;

	if(!(dom_stat = evtchn_usage[domid/NR_EVTSTAT_ENTRY_PER_BUCKET]))
		return;

	dom_stat = (evtchn_stat_t *)dom_stat + domid%NR_EVTSTAT_ENTRY_PER_BUCKET;

	ASSERT(dom_stat);

	rdom_stat = dom_stat->list_head;

	if(rdom_stat){
		rdom_evtchn_stat_t *next_rdom_stat;

		do{
			evtchn_use_stat_t *next_use_stat;
			next_rdom_stat = rdom_stat->next;
			use_stat = rdom_stat->list_head;

			if(use_stat){
				do{
					next_use_stat = use_stat->next;
					xfree(use_stat);
					use_stat = next_use_stat;
				}while(use_stat != rdom_stat->list_head);
			}

			xfree(rdom_stat);	
			rdom_stat = next_rdom_stat;

		}while(rdom_stat != dom_stat->list_head);
	}

	memset(dom_stat, 0, sizeof(evtchn_stat_t));
}

/* 
 * update_evtchn_stat:
 * 	Records new event channel or a destoryed event channel.
 * If xmalloc is not successful, 
 * there could be discrepancy between evtchn_sum and the sum of 'evtchn_count's in each rdom_evtchn_stat, 
 * and between evtchn_count and the sum of 'use_count's in each evtchn_use_stat,
 * as we count each value(evtchn_sum, evtch_count, and use_count) at different locations.
 * However, we put the most updated record first ahead of the those discrepancies.
 */
void update_evtchn_stat(domid_t domid, domid_t rdomid, uint32_t use, int operand)
{
	evtchn_stat_t *dom_stat       = NULL;
	rdom_evtchn_stat_t *rdom_stat = NULL;
	evtchn_use_stat_t *use_stat   = NULL;
	int found = 0;
	
	if(!(dom_stat = evtchn_usage[domid/NR_EVTSTAT_ENTRY_PER_BUCKET]))
		return;

	dom_stat = (evtchn_stat_t *)(dom_stat + domid%NR_EVTSTAT_ENTRY_PER_BUCKET);

	ASSERT(dom_stat);

	dom_stat->evtchn_sum += operand;

	rdom_stat = dom_stat->list_head;

	if(!rdom_stat)
		goto NEW_RDOM_MALLOC;

	do{
		if(rdom_stat->rdom == rdomid){
			found = 1;
			break;
		}else
			rdom_stat = rdom_stat->next;
	}while(rdom_stat != dom_stat->list_head);

NEW_RDOM_MALLOC:
	/* when rdom stat was not allocated yet */
	if(!found){
		rdom_stat = xmalloc(rdom_evtchn_stat_t);

		if(!rdom_stat)
			return;

		memset(rdom_stat, 0, sizeof(rdom_evtchn_stat_t));
		rdom_stat->rdom = rdomid;

		if(dom_stat->list_head == NULL){
			dom_stat->list_head = rdom_stat;
			rdom_stat->prev = rdom_stat;
			rdom_stat->next = rdom_stat;
		}else{
			rdom_evtchn_stat_t *list_tail = dom_stat->list_head->prev;
			list_tail->next = rdom_stat;
			rdom_stat->prev = list_tail;
			rdom_stat->next = dom_stat->list_head;
			dom_stat->list_head->prev = rdom_stat;
		}
	}

	rdom_stat->evtchn_count += operand;

	found = 0;
	use_stat = rdom_stat->list_head;

	/* the list is circular */
	if(!use_stat)
		goto NEW_USE_MALLOC;
	do{
		if(use_stat->use == use){
			found = 1;
			break;
		}else
			use_stat = use_stat->next;
	}while(use_stat != rdom_stat->list_head);

NEW_USE_MALLOC:
	/* when use stat was not allocated yet */
	if(!found){
		use_stat = xmalloc(evtchn_use_stat_t);

		if(!use_stat)
			return;

		memset(use_stat, 0, sizeof(evtchn_use_stat_t));
		use_stat->use = use;

		if(rdom_stat->list_head == NULL){
			rdom_stat->list_head = use_stat;
			use_stat->prev = use_stat;
			use_stat->next = use_stat;
		}else{
			evtchn_use_stat_t *list_tail = rdom_stat->list_head->prev;
			list_tail->next = use_stat;
			use_stat->prev = list_tail;
			use_stat->next = rdom_stat->list_head;
			rdom_stat->list_head->prev = use_stat;
		}
	}
	
	use_stat->use_count += operand;
}

void get_evtchn_stat(domid_t domid, domid_t rdomid, uint32_t use, int *evtchn_sum, int *rdom_evtchn_cnt, int *use_cnt)
{
	evtchn_stat_t *dom_stat       = NULL;
	rdom_evtchn_stat_t *rdom_stat = NULL;
	evtchn_use_stat_t *use_stat   = NULL;

	ASSERT(rdom_evtchn_cnt);
	ASSERT(use_cnt);

	*rdom_evtchn_cnt = -1;
	*use_cnt = -1;
	
	if(!(dom_stat = evtchn_usage[domid/NR_EVTSTAT_ENTRY_PER_BUCKET]))
		return;

	dom_stat = (evtchn_stat_t *)dom_stat + domid%NR_EVTSTAT_ENTRY_PER_BUCKET;

	ASSERT(dom_stat);

	*evtchn_sum = dom_stat->evtchn_sum;
	
	rdom_stat = dom_stat->list_head;

	if(!rdom_stat)
		return;

	do{	
		if(rdom_stat->rdom == rdomid){
			*rdom_evtchn_cnt = rdom_stat->evtchn_count;
			break;
		}else
			rdom_stat = rdom_stat->next;
	}while(rdom_stat != dom_stat->list_head);

	use_stat = rdom_stat->list_head;

	if(*rdom_evtchn_cnt<0 || !use_stat)
		return;
	do{	
		if(use_stat->use == use){
			*use_cnt = use_stat->use_count;
			break;
		}else
			use_stat = use_stat->next;
	}while(use_stat != rdom_stat->list_head);
}
