/*
 * te.c 
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

#include <security/acm/acm.h>
#include <security/acm/policy_conductor.h>
#include <security/acm/te.h>
#include <security/acm/te_extra.h>
#include <asm/arch/irqs.h>

/*
 * CURRENTLY THIS TYPE ENFORCEMENT DOES NOT DEAL WITH ACCESS CONTROL 
 * ON VCPU OBJECTS and MEMORYs.
 * THE MEMORY ACCESS REGARDING THE AMOUNT OF USE WILL BE COVERED IN OUR
 * PROPRIETARY ACDM.
 * OUR ARCHITECTURE RUNS ON A SINGLE PROCESSOR. THUS, WE DON'T CONSIDER
 * ACCESS ONTROL ON THE VCPU OBJECTS FOR NOW.
*/

/* private data structures */
struct vmlabel_ptrs{
	struct VM_Label *label_ptr;
	uint32_t *vm_to_dom_ptr;
};

/* private function declarations */
static uint16_t te_get_decision(aci_domain *subj, aci_object *obj, uint32_t req_type, aci_context *context);
static int te_set_policy(void *bin, int size);
static int te_relocate_policy(void *bin);

static struct vmlabel_ptrs vmlabel_ptr;
static struct te_policy *te_meta= NULL;
static void *te_bin = NULL;

/* private variables */
static struct acdm_ops te_ops = {
	.get_decision = te_get_decision,
	.set_policy = te_set_policy,
	.relocate_policy = te_relocate_policy,
};

static ComparisonOP te_evtchn_comp = {
	.equal = te_evtchn_equal,
	.greater = te_evtchn_greater,
	.less = te_evtchn_less,
};

static ComparisonOP te_gnttab_comp = {
	.equal = te_gnttab_equal,
	.greater = te_gnttab_greater,
	.less = te_gnttab_less,
};

static ComparisonOP te_device_comp = {
	.equal = te_device_equal,
	.greater = te_device_greater,
	.less = te_device_less,
};

static ComparisonOP te_smpres_comp = {
	.equal = te_smpres_equal,
	.greater = te_smpres_greater,
	.less = te_smpres_less,
};
 
int init_te(void)
{
	return register_decision_maker("TE", TE_MAGIC_NUMBER, &te_ops);
}

static int te_set_policy(void *bin, int size)
{
	/* 
	 * TODO: For now, we don't distinguish between the sytem-defined policy
	 * and the user-defined policy
	 */
	te_meta = bin;
	te_bin = te_meta;
	vmlabel_ptr.label_ptr = (void *)te_meta + te_meta->vm_label_offset;
	vmlabel_ptr.vm_to_dom_ptr = (void *)te_meta + te_meta->nr_vm_label*sizeof(struct VM_Label);
	printk("ACM: TE setting policy\n");

	return 1;

}

static int te_relocate_policy(void *bin)
{
	/* 
	 * TODO: For now, we don't distinguish between the sytem-defined policy
	 * and the user-defined policy
	 */
	te_meta = bin;
	te_bin = te_meta;
	vmlabel_ptr.label_ptr = (void *)te_meta + te_meta->vm_label_offset;
	vmlabel_ptr.vm_to_dom_ptr = (void *)te_meta + te_meta->nr_vm_label*sizeof(struct VM_Label);
	printk("ACM: TE relocating policy\n");

	return 1;

}


static inline uint16_t te_has_permission(uint32_t *domains, 
				 	 int nr_domains, 
					 uint32_t *types, 
					 int nr_types, 
					 objtype_policy_t *objtype_policy, 
					 uint32_t req_type)
{
	uint32_t *matrix = (uint32_t *)(te_bin + objtype_policy->matrix_offset);
	int i, j, row, column;

	for(i=0; i < nr_domains; i++){
		for(j=0; j < nr_types ; j++){
			row = domains[i];
			column = types[j];
			if((column < objtype_policy->nr_te_type) && (matrix[row*objtype_policy->nr_te_type + column] & req_type))
				return ACM_DECISION_PERMIT;
		}
	}

	return ACM_DECISION_NOTPERMIT;
}

static inline struct VM_Label *get_vm_label(unsigned long id)
{
	struct VM_Label *vmlabels, *label = NULL;
	vmlabels = te_bin + te_meta->vm_label_offset;

	if(id < te_meta->nr_te_domain && vmlabels[id].vmid == id){
		label = &vmlabels[id];
	}else{
		int i;
		for(i=0; i<te_meta->nr_te_domain; i++){
			if(vmlabels[i].vmid == id){
				label = &vmlabels[i];
				break;
			}
		}
	}

	return label;
}

static int get_access_decision(aci_domain *subj, 
				void *obj_label, 
				int label_size, 
				uint32_t req_type, 
				ComparisonOP *comp, 
				objtype_policy_t *policy)
{
	struct VM_Label *subject_label = NULL;
	void *label_list = NULL, *label_pivot = NULL;
	struct COMMON_Label *result_label = NULL;
	int pivot, lookup_size;

	ASSERT(subj);
	subject_label = get_vm_label(subj->id);

	ASSERT(policy);
	label_list = te_bin + policy->label_offset;

	/* Looks up a binary tree of simple resources' labels */
	lookup_size = policy->nr_objects;
	pivot = lookup_size/2;

	while(lookup_size > 0){
		label_pivot = label_list + pivot*label_size;

		if(comp->less(label_pivot, obj_label)){
			label_list = label_list + (pivot+1)*label_size;
			lookup_size = lookup_size - (pivot+1);
			pivot = lookup_size/2;
		}else if(comp->greater(label_pivot, obj_label)){
			lookup_size = pivot;
			pivot = lookup_size/2;
		}else if(comp->equal(label_pivot, obj_label)){
			result_label = label_pivot;
			break;
		}
	}

	/* lookup the matrix */
	if(!subject_label || !result_label)
		return ACM_DECISION_NOTPERMIT;

	return te_has_permission((uint32_t *)(te_bin + subject_label->domains_offset), 
				 subject_label->nr_domains,
				(uint32_t *)(te_bin + result_label->types_offset), 
				result_label->nr_types,
				policy, req_type);

}

static uint16_t te_get_decision(aci_domain *subj, aci_object *obj, uint32_t req_type, aci_context *context)
{
	int16_t decision = ACM_DECISION_UNDEF;

	if(!te_bin)
		return decision;

	ASSERT(obj);

	switch(obj->object_type){
		
		case ACM_PIRQ:
			if(req_type & PIRQ_CONTROL)
				goto DOM_RES;
			else if(req_type & PIRQ_SET_TYPE)
				goto SIMPLE_RES;
		{		
			struct DEVICE_Label device;
			uint16_t pirq = ((struct aci_pirq *)obj->object_info)->device;

			device.device = pirq + PIRQ_LABEL;

			decision =  get_access_decision(subj, 
						        &device, 
						        sizeof(struct DEVICE_Label), 
						        req_type, 
						        &te_device_comp, 
						        &te_meta->device_policy);
		}
			break;

		case ACM_IOMEM:
			if(req_type & IOMEM_CONTROL)
				goto DOM_RES;
		{
			struct DEVICE_Label device;

			device.device = ((struct aci_iomem *)obj->object_info)->device + IOMEM_LABEL;

			decision = get_access_decision(subj, 
						       &device, 
						       sizeof(struct DEVICE_Label), 
						       req_type, 
						       &te_device_comp, 
						       &te_meta->device_policy);
		}
			break;

		case ACM_EVENTCHN:
			if(req_type & EVTCHN_CONTROL || req_type & EVTCHN_GET_STATUS)
				goto DOM_RES;
		{
			struct EVTCHN_Label evtchn;

			evtchn.owner_index = ((struct aci_evtchn *)obj->object_info)->objdom_id.id;
			evtchn.use = ((struct aci_evtchn *)obj->object_info)->use;

			decision = get_access_decision(subj, 
						       &evtchn, 
						       sizeof(struct EVTCHN_Label), 
						       req_type, 
						       &te_evtchn_comp, 
						       &te_meta->evtchn_policy);

			if(decision == ACM_DECISION_NOTPERMIT){
				aci_domain tmp;
				tmp.id = evtchn.owner_index;
				evtchn.owner_index = subj->id;
				evtchn.use = ((struct aci_evtchn *)obj->object_info)->use;

				decision = get_access_decision(&tmp, 
							       &evtchn, 
							       sizeof(struct EVTCHN_Label), 
							       req_type, 
							       &te_evtchn_comp, 
							       &te_meta->evtchn_policy);
			
			}
		}
			break;

		case ACM_GRANTTAB:
			if(req_type & GNTTAB_SETUP || req_type & GNTTAB_DUMP_TABLE)
				goto DOM_RES;
		{
			struct GNTTAB_Label gnttab;

			gnttab.owner_index = ((struct aci_gnttab *)obj->object_info)->objdom_id.id;
			gnttab.use = ((struct aci_gnttab *)obj->object_info)->use;
			gnttab.mem_space = ((struct aci_gnttab *)obj->object_info)->mem_space;

			decision = get_access_decision(subj, 
						       &gnttab, 
						       sizeof(struct GNTTAB_Label), 
						       req_type, 
						       &te_gnttab_comp, 
						       &te_meta->gnttab_policy);
		}
			break;	

		case ACM_VIRQ:
		{
			struct SIMPLE_RESOURCE_Label virq;

			virq.value = ((struct aci_virq *)obj->object_info)->virq;

			decision = get_access_decision(subj, 
				  		       &virq, 
						       sizeof(struct SIMPLE_RESOURCE_Label), 
						       req_type, 
						       &te_smpres_comp, 
						       &te_meta->virq_policy);
		}
			break;

		case ACM_MEMORY:
			if(req_type == MEM_CONTROL || req_type == MEM_GET_STAT ||
		           req_type == MEM_TRANSLATE_ADDR)
				goto DOM_RES;
			break;

		case ACM_VCPU:
			decision = ACM_DECISION_UNDEF;

			break;

		case ACM_TIME:
		case ACM_SCHEDULER:
		case ACM_TRACEBUFFER:
		case ACM_PERFCOUNTER:
		case ACM_CONSOLEIO:

		/*
		 * TODO: TEMPORARILY CHANGED 
		 * TIME~CONSOLEIO : SIMPLE_RESOURCE_Label
		 */
			decision = ACM_DECISION_UNDEF;

			break;

		case ACM_POLICY:	
		case ACM_SECUSTORAGE:
		case ACM_ETC:
SIMPLE_RES:			
		{	
			struct SIMPLE_RESOURCE_Label smpres;

			smpres.value = obj->object_type;
			req_type = shift_request(req_type, obj->object_type);

			decision = get_access_decision(subj, 
						       &smpres, 
						       sizeof(struct SIMPLE_RESOURCE_Label), 
						       req_type, 
						       &te_smpres_comp, 
						       &te_meta->simple_res_policy);
		}
			break;

		case ACM_DEBUG:
		case ACM_HANDLE:

		/* 
		 * TODO: TEMPORARILY CHANGED 
		 * DEBUG, HANDLE : DOM_RES
		 */
		case ACM_DOMAIN:	
DOM_RES:
		{
			struct SIMPLE_RESOURCE_Label domres;
			uint32_t val = 0;

			domres.value = obj->object_type;
			val = ((aci_domain *)obj->object_info)->id;
			domres.value = ( val << 16) + domres.value; 
			req_type = shift_request(req_type, obj->object_type);

			decision = get_access_decision(subj, 
						       &domres, 
						       sizeof(struct SIMPLE_RESOURCE_Label), 
						       req_type, 
						       &te_smpres_comp, 
						       &te_meta->domain_res_policy);
		}
			break;
			
		default:
			decision = ACM_DECISION_UNDEF;

			break;
	}

	return decision;
}
