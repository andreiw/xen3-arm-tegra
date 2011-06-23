/*
 * acm_integrator.c 
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
#include <security/acm/decision_cache.h>
#include <security/acm/policy_conductor.h>
#include <xen/lib.h>

extern struct ac_model ac_models[ACM_MODEL_MAX];
extern int ac_model_count;

/*
 * lookup_cnt 	: the number of models inspected so far
 * result 	: returns the number of decision result indicating either permit or undefined. 
 *		  returns 0 if not granted
 *
 */

/* TODO: the option turning on/off cache should be given to each ACDM! */
int acm_is_granted(aci_domain *subject, aci_object *object, uint32_t request_type, aci_context *context){
	int i, lookup_cnt = 0;
	uint16_t cache, decision, new_decision, cache_update;
	uint8_t key_str[ACMCACHE_KEY_SIZE];
	uint32_t key;
	int result = 0;
	struct acdm_ops *ops = NULL;
	int update = 0;
	acm_caching_info caching;

	get_key_string(subject, object, request_type, key_str);
	key = constitute_key(key_str);

	cache = lookup_cache(key_str, key, &caching);
	cache_update = cache;

	for(i=0; i<ACM_MODEL_MAX && lookup_cnt < ac_model_count; i++){
		/* get the decisions from each access control model */
		decision = (cache & (ACMCACHE_DECISION_RMASK << i*ACMCACHE_DECISION_BITS)) >> i*ACMCACHE_DECISION_BITS;	

		if(decision == ACMCACHE_NOCACHE){
			ops = ac_models[i].ops;
			if(ops != NULL){
				new_decision = ops->get_decision(subject, object, request_type, context);
				decision = ACMCACHE_DECISION_RMASK & new_decision;

				/* In case of cache update */
				if(!(new_decision & ACMCACHE_NOCACHE_MASK)){
					cache_update |= (decision << i*ACMCACHE_DECISION_BITS);
					update = 1;
				}

				lookup_cnt++;
			}

		}else{
			lookup_cnt++;
		}

		if(decision == ACM_DECISION_NOTPERMIT)
			result -= 1000;
		else
			result++;
	}

	if(update){
		update_cache(key_str, key, -1, cache_update, &caching);
	}

	if(result <= 0){
		if(object->object_type != ACM_IOMEM)
		printk("ACM_NOT_PERMIT -------------------subject:%lu object_type:%x request_type:%x\n", subject->id, object->object_type, request_type);
		result = 0;
	}

	return result;
}

/* FOR CPU SCHEDULING */
void acm_control_cpu_usage(struct domain *d)
{
	int i;
	struct acdm_ops *ops = NULL;

	for(i=0; i<ac_model_count; i++){
		ops = ac_models[i].ops;
		if(ops != NULL && ops->control_cpu_usage != NULL){
			ops->control_cpu_usage(d);
		}
	}
}

/* Checks the battery usage policy in the battery unit of 5% */
int acm_control_battery_usage(unsigned int batterylife, struct domain *d)
{
	int i;
	uint16_t result = 0;
	struct acdm_ops *ops = NULL;

	for(i=0; i<ac_model_count; i++){
		ops = ac_models[i].ops;

		if(ops != NULL && ops->control_battery_usage != NULL){
			result |= ops->control_battery_usage(d, batterylife);
		}
	}

	return result;
}
