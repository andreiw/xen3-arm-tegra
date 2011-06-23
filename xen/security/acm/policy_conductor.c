/*
 * policy_conductor.c  
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          SungMin Lee    <sung.min.lee@samsung.com>
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

#include <xen/errno.h>
#include <security/acm/acm.h>
#include <security/acm/policy_conductor.h>
#include <security/acm/decision_cache.h>
#include <security/acm/acm_hooks.h>
#include <xen/string.h>
#include <xen/guest_access.h>
#include <public/acm_ops.h>
#include <security/ssm-xen/sra_func.h>
#include <xen/sched.h>
#include <xen/event.h>
#include <public/xen.h>
#include <xen/iocap.h>

static int init_acdms(void);
static void flush_physical_resource_permission(void);

struct ac_model ac_models[ACM_MODEL_MAX];
int ac_model_count;
struct vcpu *drvdom_vcpu = NULL;

int init_acm(void)
{
	memset(ac_models, 0, sizeof(struct ac_model)*ACM_MODEL_MAX);

	ac_model_count = 0;

	init_acdms();
	init_decision_cache();

	return 1;
}

extern int init_proprietary(void);
extern int init_blp(void);

static int init_acdms(void)
{
	init_te();
	init_proprietary();
	
	return 1;
}

/* Load policy from the secure storage for the registered ith acdm*/
inline void load_policy(int idx)
{
	default_struct_t *policy_image = NULL;

	struct acm_policy_header *policy_header = NULL;
	int i, j, candidate;

	candidate = idx + ACM_MODEL_MAX - 1;

	ac_models[idx].storage_index = -1;
	ac_models[idx].policy_bin = NULL;
	ac_models[idx].bin_size = 0;
	ac_models[idx].version = -1;

	/* search from 'idx'th storage index to reduce search time */
	for(i = idx; i < idx+ACM_MODEL_MAX; i++){

		j = i%ACM_MODEL_MAX; 

		policy_image = sra_get_image(PART_SP2, XEN_AC_POLICY_1 + j);

		if(policy_image && policy_image->size > 0){
			policy_header = (struct acm_policy_header *)policy_image->u.ptr;

			if(!policy_header || policy_header->acdm_magic_num != ac_models[idx].magic_num)
				continue;

			ac_models[idx].ops->set_policy((void *)policy_header + sizeof(struct acm_policy_header),
							policy_header->bin_size - sizeof(struct acm_policy_header));
			ac_models[idx].policy_bin = policy_header;
			ac_models[idx].bin_size = policy_header->bin_size;
			ac_models[idx].storage_index = j;
			ac_models[idx].version = policy_header->version;

			break;

		}else if(candidate > i){
			candidate = i;
		}
	}

	if(ac_models[idx].storage_index < 0)
		ac_models[idx].storage_index = candidate % ACM_MODEL_MAX;
}

int register_decision_maker(char *model_name, uint32_t magic_num, struct acdm_ops *ops)
{
	int success = 0;
	int i, idx;

	if(ac_model_count < ACM_MODEL_MAX){

		for(i=0; i<ACM_MODEL_MAX; i++){

			idx = (ac_model_count + i) % ACM_MODEL_MAX;

			if(ac_models[idx].ops == NULL){
				printk("acm: registering decision maker %s at %d ", model_name, idx);

				strncpy(ac_models[idx].model_name, model_name, ACM_MODELNAME_MAXLEN);
				ac_models[idx].magic_num = magic_num;
				ac_models[idx].ops = ops;

				/* Load policy from the secure storage */
				load_policy(idx);

				ac_model_count++;
				success = 1;
				printk("acm: total number of models: %d \n ",ac_model_count);

				break;
			}
		}
	}else
		printk("ACM: acm model overflow\n");

	return success;
}
	
struct ac_model* get_ac_models_ptr(void)
{
	return ac_models;
}

/* Load a policy to the corresponding ACDM.
   If success, revoke previous policy. */
static long set_vmm_policy(struct acm_policy_header *policy_header, char *bin, uint32_t size)
{
	int i;
	long ret = -1;

	if(!bin)
		return ret;

	if(policy_header->pc_magic_num != PC_MAGIC_NUM || policy_header->bin_size < sizeof(struct acm_policy_header))
		return ret;

	for(i =0; i < ac_model_count; i++){
		if(policy_header->acdm_magic_num == ac_models[i].magic_num){

			int j;
			default_struct_t policy_image, *stored_policy = NULL;

			/* Store in the on-memory storage */
			printk("ACM: received model[%i]:%s's policy\n", i, ac_models[i].model_name);

			policy_image.type = XEN_AC_POLICY_1 + i;
			policy_image.size = size;
			policy_image.u.ptr = bin;

			if(sra_set_image(&policy_image, PART_SP2))
				return ret;

			stored_policy = sra_get_image(PART_SP2, XEN_AC_POLICY_1 + i);

			if(!stored_policy)
				return ret;

			bin = stored_policy->u.ptr;

			/* update policy */
			if(bin && stored_policy->size > 0 &&
				ac_models[i].ops->set_policy((void *)bin + sizeof(struct acm_policy_header), 
							policy_header->bin_size - sizeof(struct acm_policy_header)) )
			{
				ac_models[i].policy_bin = bin;
				ac_models[i].bin_size = size;
				ac_models[i].version = policy_header->version;
				ret = i;
			}else{
				ac_models[i].policy_bin = NULL;
				ac_models[i].bin_size = 0;
				ac_models[i].version = -1;
			}

			flush_cache(i);
			flush_physical_resource_permission();

			/* 
			 * During sra_set_image, our policy binaries may have been relocated.
			 * However, their contents will not change as long as there isn't an error.
			 * Thus, we notify each ACDM that its policy has been relocated.
			 */
			for(j =0; j < ac_model_count; j++){

				if(i == j)
					continue;

				stored_policy = sra_get_image(PART_SP2, XEN_AC_POLICY_1 + j);

				if(!stored_policy || !(bin = stored_policy->u.ptr))
					continue;

				/* relocate policy */
				if(stored_policy->size > 0 &&
					ac_models[j].ops->relocate_policy((void *)bin + sizeof(struct acm_policy_header)) )
				{
					ac_models[j].policy_bin = bin;
				}else{
					ac_models[j].policy_bin = NULL;
					ac_models[j].bin_size = 0;
					printk("ACM: ERROR ON SECURE STORAGE AFTER CALLING sra_get_image!!\n");
				}
			}

			break;
		}
	}

	return ret;
}

static long set_drvdom_policy(struct acm_policy_header *policy_header, char *bin, uint32_t size)
{
	long ret = -1;
	default_struct_t policy_image;

	if(!bin ||
	   policy_header->pc_magic_num != PC_MAGIC_NUM || 
	   policy_header->acdm_magic_num != DDOM_MAGIC_NUM ||
	   policy_header->bin_size < sizeof(struct acm_policy_header))
		return ret; 

	policy_image.type = DRIVER_DOM_AC_POLICY;
	policy_image.size = size;
	policy_image.u.ptr = bin;

	if(!sra_set_image(&policy_image, PART_SP2)){
		ret = 0;
		 
		/* The first argument should be the id of driver domain. */
		if(drvdom_vcpu)
			send_guest_virq(drvdom_vcpu, VIRQ_DDOM_POLICY);
	}

	return ret;
}

#ifndef ACM_SECURITY
long do_acm_op(GUEST_HANDLE(acm_op_t) u_acm_op)
{
	long ret = 0;
	struct acm_op op;

	static spinlock_t acm_lock = SPIN_LOCK_UNLOCKED;

	printk("ACM: do_acm_op\n");
		
	if(copy_from_guest(&op, u_acm_op, 1))
		return -EFAULT;

	spin_lock(&acm_lock);

	switch(op.cmd){
		case ACM_GETVERSION:
			{
				if(!acm_access_policy(op.u.getversion.type_flag)){
					ret = -EPERM;
					printk("acm_access_policy returned -EPERM\n");
					break;
				}
				printk("type_flag: %08X (VMM:%08X, DRVDOM:%08X)\n", op.u.getversion.type_flag, VMM, DRVDOM);
				switch(op.u.getversion.type_flag) {
					case VMM:
						{
							int i;
							struct acm_policy_header *ph;
							printk("policy search(magic_num=%08X)\n", op.u.getversion.magic_num);
							for(i=0; i<ACM_MODEL_MAX; i++){

								if (ac_models[i].model_name[0] > 0) {
									printk("check policy(%s, idx=%d) magic:%08X, version: %d\n", 
											ac_models[i].model_name, 
											i, ac_models[i].magic_num, ac_models[i].version);
								}

								if (ac_models[i].magic_num == op.u.getversion.magic_num) {
									op.u.getversion.version = ac_models[i].version;
									copy_to_guest(u_acm_op, &op, 1);					
									printk("founded policy(%s), version: %d\n", ac_models[i].model_name, ac_models[i].version);
									break;
								}
							}
							break;
						}
					case DRVDOM:
						{
							printk("DRVDOM not implemented\n");
							break;
						}
				}
				break;
			}
		case ACM_SETPOLICY:
		{
			struct acm_policy_header policy_header;
			GUEST_HANDLE(void) upolicy_header;

			if(!acm_access_policy(op.u.setpolicy.type_flag)){
				ret = -EPERM;
				break;
			}

			upolicy_header.p = op.u.setpolicy.binpolicy;

			if(copy_from_guest(&policy_header, upolicy_header, 1) 
				|| policy_header.bin_size != op.u.setpolicy.policy_size){
				ret = -EFAULT;
				break;
			}

			switch(op.u.setpolicy.type_flag){
				case VMM:
					if(set_vmm_policy(&policy_header, op.u.setpolicy.binpolicy, op.u.setpolicy.policy_size) < 0 )
						ret = -EFAULT;
					break;
				case DRVDOM:
					if(set_drvdom_policy(&policy_header, op.u.setpolicy.binpolicy, op.u.setpolicy.policy_size) < 0 )
						ret = -EFAULT;
					break;
			}
		}
			break;
		

		case ACM_GETPOLICY:
		{
			int idx = -1;

			if(!acm_access_policy(op.u.getpolicy.type_flag)){
				ret = -EPERM;
				break;
			}

			switch(op.u.getpolicy.type_flag){
				case VMM:
					if(op.u.getpolicy.index < ACM_MODEL_MAX)
						idx =  XEN_AC_POLICY_1 + ac_models[op.u.getpolicy.index].storage_index;
					break;

				case DRVDOM:
					idx = DRIVER_DOM_AC_POLICY; 	
					break;
			}

			if(idx > 0){

				default_struct_t *policy_image;
				GUEST_HANDLE(void) upolicy;
				op.u.getpolicy.result = 0;

				upolicy.p = op.u.getpolicy.binpolicy;
				policy_image = sra_get_image(PART_SP2, idx);

				if(policy_image && policy_image->size > 0){

					if(policy_image->size <= op.u.getpolicy.alloc_size)
						copy_to_guest(upolicy, policy_image->u.ptr, policy_image->size);
					else{
						op.u.getpolicy.result = -1;
					}

					op.u.getpolicy.policy_size = policy_image->size;

				}else{
					op.u.getpolicy.policy_size = 0;
				}

				copy_to_guest(u_acm_op, &op, 1);
			}else

			ret = -EFAULT;
		}
			break;
		
		case ACM_GETACDMLIST:
			if(!acm_access_policy(op.u.getacdmlist.type_flag)){
				ret = -EPERM;
				break;
			}

			switch(op.u.getacdmlist.type_flag){
				case VMM:
				{	
					int i;
					acdminfo_t acdminfo;

					for(i=0; i<ACM_MODEL_MAX; i++){
						strncpy(acdminfo.model_name, ac_models[i].model_name, ACM_MODELNAME_MAXLEN);
						acdminfo.magic_num = ac_models[i].magic_num;
						acdminfo.policy_size = ac_models[i].bin_size;
						copy_to_guest_offset(op.u.getacdmlist.buffer, i, &acdminfo, 1);
					}

					op.u.getacdmlist.acdm_count = ac_model_count; 
					copy_to_guest(u_acm_op, &op, 1);
				}

					break;
				
				case DRVDOM:
				{
					acdminfo_t acdminfo;
					default_struct_t *policy_image;
					policy_image = sra_get_image(PART_SP2, DRIVER_DOM_AC_POLICY);

					if(!policy_image)
						ret = -EFAULT;
					else{
						acdminfo.policy_size = policy_image->size;
						copy_to_guest_offset(op.u.getacdmlist.buffer, 0, &acdminfo, 1);
						op.u.getacdmlist.acdm_count = 1; 
						copy_to_guest(u_acm_op, &op, 1);
					}
				}
					break;
				
			}
			break;
		
		default:

		ret = -EFAULT;

		break;
	}

	spin_unlock(&acm_lock);

	return ret;
}
#endif

static void flush_physical_resource_permission(void)
{
	struct domain *d = NULL;
	int ioport_ret = 0,
	    iomem_ret  = 0,
            irqs_ret   = 0;

	read_lock(&domlist_lock);

	for_each_domain ( d ){
		ioport_ret = ioports_deny_access(d, 0, 0xFFFF);
		iomem_ret = iomem_deny_access(d, 0UL, ~0UL);
		irqs_ret = irqs_deny_access(d, 0, NR_PIRQS-1);
	}

	read_unlock(&domlist_lock);
}
