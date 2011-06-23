/*
 * decision_cache.c 
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
 * Decision cache doesn't provide entry deletion operations. 
 * The cache entry will be always replaced with another value.
*/

#include <security/acm/decision_cache.h>
#include <security/acm/policy_conductor.h>
#include <xen/string.h>

extern struct ac_model ac_models[ACM_MODEL_MAX];
extern int ac_model_count;

decision_cache *acm_cache_table;

/* an indicator for LRU per bucket */
struct decision_lru_cache lru_cache_info[ACMCACHE_TOTAL_BUCKET];
struct decision_lru_cache *cache_info_ptr;

int init_decision_cache()
{
	cache_info_ptr = (struct decision_lru_cache *)memset(lru_cache_info, 0,
							     sizeof(struct decision_lru_cache)*ACMCACHE_TOTAL_BUCKET);
	acm_cache_table = alloc_xenheap_pages(ACMCACHE_ORDER);

	if(acm_cache_table == NULL)
		return 0;

	acm_cache_table = (decision_cache *)memset(acm_cache_table, 0, ACMCACHE_MEM);

	printk("ACM: total bucket:%lu bucket_size:%d elements:%lu d_cache size:%d memsize:%lu\n", 
		ACMCACHE_TOTAL_BUCKET, ACMCACHE_BUCKET_SIZE, ACMCACHE_BUCKET_ELEMENTS, sizeof(decision_cache), ACMCACHE_MEM);

	return 1;
}

uint16_t lookup_cache(uint8_t *key_str, uint32_t key, acm_caching_info *caching)
{
	uint32_t bucket_idx, bucket_offset;
	decision_cache *cache_entry = NULL;

	ASSERT(caching);

	caching->cache_entry = NULL;

	if(!lru_cache_info || !acm_cache_table)
		return ACMCACHE_NOCACHE;

	bucket_idx = key % ACMCACHE_TOTAL_BUCKET;
	caching->bucket_idx = bucket_idx;
	bucket_offset = key & ~ACMCACHE_BUCKET_MASK;
	caching->bucket_offset = bucket_offset;

	/* check if cache has no available entry */
	if(lru_cache_info[bucket_idx].vacancy == 0){
		return ACMCACHE_NOCACHE;
	}
		
	cache_entry = (decision_cache *)(acm_cache_table + bucket_idx*ACMCACHE_BUCKET_ELEMENTS + bucket_offset);

	if(!memcmp(cache_entry->key, key_str, ACMCACHE_KEY_SIZE)){
		caching->cache_entry = cache_entry;

		return cache_entry->decisions;
	}else{
		/* search sequentially within a bucket */
		int i, j;

		decision_cache *bucket_start = NULL;
		bucket_start = cache_entry - bucket_offset;

		for(i = 1; i < ACMCACHE_BUCKET_ELEMENTS; i++){
			j = (bucket_offset + i)%ACMCACHE_BUCKET_ELEMENTS;
			cache_entry = (decision_cache *)(bucket_start + j);

			if(!memcmp(cache_entry->key, key_str, ACMCACHE_KEY_SIZE)){
				/* hit cache by the sequential search */
				caching->cache_entry = cache_entry;

				return cache_entry->decisions;
			}
		}
	}
	
	/* cache miss */
	return ACMCACHE_NOCACHE;
}

int update_cache(uint8_t *key_str, uint32_t key, int  acmodel_idx, uint16_t val, acm_caching_info *caching)
{
	uint32_t bucket_idx, bucket_offset, hit = 0;

	decision_cache *cache_entry       = NULL, 
                       *update_entry      = NULL, 
                       *replace_candidate = NULL;

	struct decision_lru_cache *bucket_info = NULL;
	
	if(!lru_cache_info || !acm_cache_table)
		return -1;

	bucket_idx    = caching->bucket_idx;
	bucket_offset = caching->bucket_offset;
	cache_entry   = caching->cache_entry;

	/* if cache entry is empty or maches with the key, that's the entry to be updated */
	if(cache_entry && !memcmp(cache_entry->key, key_str, ACMCACHE_KEY_SIZE)){
		update_entry = cache_entry;
		hit = 1;
	}else{
		cache_entry = (decision_cache *)(acm_cache_table + bucket_idx*ACMCACHE_BUCKET_ELEMENTS + bucket_offset);
		bucket_info = &lru_cache_info[bucket_idx];

		if(bucket_info->vacancy < ACMCACHE_BUCKET_ELEMENTS 
			&& cache_entry->decisions == ACMCACHE_NOCACHE){
			/* found vacant entry */
			bucket_info->vacancy++;	
			update_entry = cache_entry;
		}else{
			/* search sequentially within a bucket
		 	* find an empty entry or the entry to be replaced
		 	*/
			decision_cache *bucket_start, *bucket_end, *tmp_entry;
			bucket_start = cache_entry - bucket_offset;
			bucket_end = bucket_start + ACMCACHE_BUCKET_ELEMENTS -1;
			tmp_entry = cache_entry;

			do{
				if(bucket_info->vacancy < ACMCACHE_BUCKET_ELEMENTS && 
				   tmp_entry->decisions == ACMCACHE_NOCACHE){
					bucket_info->vacancy++;	
					update_entry = tmp_entry;
					break;

				}else if(tmp_entry->index == bucket_info->next_replace){
					replace_candidate = tmp_entry;
				}

				tmp_entry++;

				if(tmp_entry > bucket_end)
					tmp_entry = bucket_start;

			}while(tmp_entry != cache_entry);
		}
	}

	/* updates cache entry */
	if( update_entry == NULL){
		if(replace_candidate != NULL){
			/* replace the entry at lru_cache_info[bucket_index].next_replace */
			update_entry = replace_candidate;
		}else{
			printk("ACM: somthing wrong in cache table\n");
			return -1;
		}
	}

	if(!hit){
		memcpy(update_entry->key, key_str, ACMCACHE_KEY_SIZE);
		update_entry->index = bucket_info->next_replace;
		bucket_info->next_replace = (update_entry->index + 1) % (ACMCACHE_BUCKET_ELEMENTS);
	}

	if(acmodel_idx == -1)
		update_entry->decisions = val;
	else{
		/* updates only a specific acmodel's decision */
		uint16_t bitmask;
		uint16_t origin_decisions = update_entry->decisions;

		bitmask = ACMCACHE_DECISION_RMASK << acmodel_idx;
		update_entry->decisions = (origin_decisions & ~bitmask) | ((val << acmodel_idx) & bitmask);
	}

	return 1;
}

/* TODO: change constitute key */
uint32_t constitute_key(uint8_t *key_str)
{
	uint32_t hashed_key = 0;
	uint32_t subkey = 0;
	int i;

	for(i=0; i<=ACMCACHE_KEY_SIZE-sizeof(uint32_t); i+=sizeof(uint32_t)){
		subkey = *(uint32_t *)(key_str + i);

		if(subkey)
			hashed_key ^= subkey;
	}
	if(i != ACMCACHE_KEY_SIZE){
		subkey = 0;
		memcpy((char *)&subkey, key_str + i, ACMCACHE_KEY_SIZE - i);

		if(subkey)
			hashed_key ^= subkey;
	}
	return hashed_key;
}

/* TODO: finish key string generation for other object types too. */
void get_key_string(aci_domain *subject, aci_object *object, uint32_t request_type, uint8_t *key_str)
{
	uint8_t *ptr = key_str;

	memset(key_str, 0, ACMCACHE_KEY_SIZE);
	memcpy(ptr, &subject->id, ACI_SUBJID_SIZE);

	ptr += ACI_SUBJID_SIZE;

	ASSERT(object);

	memcpy(ptr, &(object->object_type), sizeof(objtype_t)); 

	ptr += sizeof(objtype_t);

	/*
	 * OBJST_TYPE_DOMAIN:
	 * operation types below are about access to resource of a domain
	 * VCPU_CONTROL, MEM_CONTROL, MEM_GET_STAT, MEM_TRANSLATE_ADDR,
	 * PIRQ_CONTROL, IOMEM_CONTROL, EVTCHN_CONTROL, EVTCHN_GET_STAT
	 * GNTTAB_SETUP, GNTTAB_DUMP_TABLE, MEM_GET_STAT, MEM_TRANSLATE_ADDR,
	 * HANDLE_SET, DEBUG_SET
	 *
	 * OBJST_TYPE_SIMPLE: whose object_info field is NULL.
	 */
	if(object->struct_type == OBJST_TYPE_ONDOMAIN){
			memcpy(ptr, (struct aci_domain *)object->object_info, ACI_SUBJID_SIZE);
			ptr += sizeof(ACI_SUBJID_SIZE);
	}else if(object->struct_type != OBJST_TYPE_SIMPLE){
		switch(object->object_type){

			case ACM_EVENTCHN:
				memcpy(ptr, (struct aci_evtchn *)object->object_info, ACI_EVTCHN_KEYSTR_SIZE);
				ptr += ACI_EVTCHN_KEYSTR_SIZE;
				break;

			case ACM_PIRQ: 
				memcpy(ptr, (struct aci_pirq *)object->object_info, ACI_PIRQ_KEYSTR_SIZE);
				ptr += ACI_PIRQ_KEYSTR_SIZE;
				break;

			case ACM_IOMEM:
				memcpy(ptr, (struct aci_iomem *)object->object_info, ACI_IOMEM_KEYSTR_SIZE);
				ptr += ACI_IOMEM_KEYSTR_SIZE;
				break;

			case ACM_GRANTTAB:
				memcpy(ptr, (struct aci_gnttab *)object->object_info, ACI_GNTTAB_KEYSTR_SIZE);
				ptr += ACI_GNTTAB_KEYSTR_SIZE;
				break;

			case ACM_DOMAIN:
				memcpy(ptr, (struct aci_domain *)object->object_info, ACI_SUBJID_SIZE);
				ptr += sizeof(ACI_SUBJID_SIZE);
				break;

			case ACM_VCPU:
				memcpy(ptr, (struct aci_virq *)object->object_info, ACI_VCPU_KEYSTR_SIZE);
				ptr += ACI_VCPU_KEYSTR_SIZE;
				break;

			case ACM_VIRQ:
				memcpy(ptr, (struct aci_virq *)object->object_info, ACI_VIRQ_KEYSTR_SIZE);
				ptr += ACI_VIRQ_KEYSTR_SIZE;
				break;

			case ACM_MEMORY:
				memcpy(ptr, (struct aci_memory *)object->object_info, ACI_MEMORY_KEYSTR_SIZE);
				ptr += ACI_MEMORY_KEYSTR_SIZE;
				break;

			default:
				printk("[decision_cache] Unclassified object type!\n");
				break;
		}
	}

	if(key_str + ACMCACHE_KEY_SIZE < ptr+4)
		printk("ACM: MEMORY OVERFLOW!!! key_str:%p ptr:%p %dbytes keysize:%d\n", 
						key_str, ptr, ptr-key_str, ACMCACHE_KEY_SIZE);
	else
		memcpy(ptr, &request_type, ACI_REQTYPE_SIZE);
}


void print_map_cache()
{
	int i, j;
	decision_cache *cache_entry = NULL;
	struct decision_lru_cache *bucket_lru_info = NULL;

	cache_entry = acm_cache_table;
	bucket_lru_info = lru_cache_info; 
	printk("ACM: cache_table print: %p \n", acm_cache_table);	

	for(i=0; i<ACMCACHE_TOTAL_BUCKET; i++){
		printk("ACM: bucket:%d next_replace:%u vacancy:%d\n", 
							i, bucket_lru_info->next_replace, bucket_lru_info->vacancy);
		for(j=0; j<ACMCACHE_BUCKET_ELEMENTS; j++){
			printk("ACM: \t[%p] index:%u key:%s decisions:%u\n", 
							cache_entry, cache_entry->index, cache_entry->key, cache_entry->decisions);
			cache_entry++;
		}

		bucket_lru_info++;
	}	
}

void flush_cache(int index)
{
	int i, j;
	uint16_t mask;
	decision_cache *cache_entry = NULL;
	cache_entry = acm_cache_table;

	if(index > ACM_MODEL_MAX)
		return;

	if(index < 0)
		mask = 0;
	else
		mask = 	~(ACMCACHE_DECISION_RMASK << index*ACMCACHE_DECISION_BITS);

	for(i=0; i<ACMCACHE_TOTAL_BUCKET; i++){
		for(j=0; j<ACMCACHE_BUCKET_ELEMENTS; j++){
			cache_entry->decisions = cache_entry->decisions & mask;	
			cache_entry++;
		}
	}
}
