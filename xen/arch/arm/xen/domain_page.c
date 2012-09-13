/*
 * domain_page.c
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
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

#include <xen/lib.h>
#include <xen/sched.h> 
#include <xen/perfc.h>
#include <xen/domain_page.h>
#include <public/arch-arm.h>
#include <asm/current.h>
#include <asm/config.h>
#include <asm/domain.h>
#include <asm/hardirq.h>
#include <asm/memory.h>

#ifdef CONFIG_DOMAIN_PAGE

struct mapcache mapcache;

#if 0
static unsigned long inuse[BITS_TO_LONGS(GLOBALMAP_BITS)];
static unsigned long garbage[BITS_TO_LONGS(GLOBALMAP_BITS)];
static unsigned int inuse_cursor;
static spinlock_t globalmap_lock = SPIN_LOCK_UNLOCKED;
#endif

void *map_domain_page(unsigned long pfn)
{
	unsigned int idx, i;
	unsigned long va;

    ASSERT(!in_irq());

	spin_lock(&mapcache.lock);

	idx = find_next_zero_bit(mapcache.inuse, MAPCACHE_ENTRIES, mapcache.cursor);
	if ( unlikely(idx >= MAPCACHE_ENTRIES) )
	{
		
		/* /First/, clean the garbage map and update the inuse list. */
		for ( i = 0; i < ARRAY_SIZE(mapcache.garbage); i++ )
		{
			unsigned long x = xchg(&mapcache.garbage[i], 0);
			mapcache.inuse[i] &= ~x;
		}

		idx = find_first_zero_bit(mapcache.inuse, MAPCACHE_ENTRIES);
		ASSERT(idx < MAPCACHE_ENTRIES);
	}

	set_bit(idx, mapcache.inuse);
	mapcache.cursor = idx + 1;

	spin_unlock(&mapcache.lock);

    consistent_write(&mapcache.table[idx], ((pfn << PAGE_SHIFT) | PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_UNO_SRW));

	va = MAPCACHE_VIRT_START + (idx << PAGE_SHIFT);
    cpu_flush_tlb_entry(va);

	return (void *)va;
}

void unmap_domain_page(void *va)
{
	unsigned int idx;

	ASSERT((void *)MAPCACHE_VIRT_START <= va);
	ASSERT(va < (void *)MAPCACHE_VIRT_END);

	idx = ((unsigned long)va - MAPCACHE_VIRT_START) >> PAGE_SHIFT;
	
	consistent_write(&mapcache.table[idx], 0);
	cpu_flush_tlb_entry(va);

	set_bit(idx, mapcache.garbage);
}

#if 0
static inline struct vcpu *mapcache_current_vcpu(void)
{
	return current;
}
#endif

#if 0
void *map_domain_page(unsigned long pfn)
{
    unsigned long va;
    unsigned int idx, i, vcpu;
#if 0
    struct vcpu *v;
    struct mapcache *cache;
#endif

    struct vcpu_maphash_entry *hashent;

    ASSERT(!in_irq());

    v = mapcache_current_vcpu();

    vcpu  = v->vcpu_id;
    cache = &v->domain->arch.mapcache;

    hashent = &cache->vcpu_maphash[vcpu].hash[MAPHASH_HASHFN(pfn)];
    if ( hashent->pfn == pfn )
    {
        idx = hashent->idx;
        hashent->refcnt++;

        ASSERT(hashent->refcnt != 0);
        ASSERT((pte_val(cache->l1tab[idx]) >> PAGE_SHIFT) == pfn);

        goto out;
    }

    spin_lock(&cache->lock);

    idx = find_next_zero_bit(cache->inuse, MAPCACHE_ENTRIES, cache->cursor);
    if ( unlikely(idx >= MAPCACHE_ENTRIES) )
    {
        /* /First/, clean the garbage map and update the inuse list. */
        for ( i = 0; i < ARRAY_SIZE(cache->garbage); i++ )
        {
            unsigned long x = xchg(&cache->garbage[i], 0);
            cache->inuse[i] &= ~x;
        }

        idx = find_first_zero_bit(cache->inuse, MAPCACHE_ENTRIES);
        ASSERT(idx < MAPCACHE_ENTRIES);
    }

    set_bit(idx, cache->inuse);
    cache->cursor = idx + 1;

    spin_unlock(&cache->lock);

 //   cache->l1tab[idx] = l1e_from_pfn(pfn, PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_UNO_SRW);
	consistent_write(&cache->l1tab[idx], ((pfn << PAGE_SHIFT) | PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_UNO_SRW));

//	cpu_clean_cache_range((unsigned long)(&cache->l1tab[idx]), 32);

 out:
    va = MAPCACHE_VIRT_START + (idx << PAGE_SHIFT);
	cpu_flush_tlb_entry(va);

    return (void *)va;
}

void unmap_domain_page(void *va)
{
    unsigned int idx;
    struct vcpu *v;
    struct mapcache *cache;
    unsigned long pfn;
    struct vcpu_maphash_entry *hashent;

    ASSERT(!in_irq());

    ASSERT((void *)MAPCACHE_VIRT_START <= va);
    ASSERT(va < (void *)MAPCACHE_VIRT_END);

    v = mapcache_current_vcpu();

    cache = &v->domain->arch.mapcache;

    idx = ((unsigned long)va - MAPCACHE_VIRT_START) >> PAGE_SHIFT;

    pfn = pte_val(cache->l1tab[idx]) >> PAGE_SHIFT;
    hashent = &cache->vcpu_maphash[v->vcpu_id].hash[MAPHASH_HASHFN(pfn)];

    if ( hashent->idx == idx )
    {
        ASSERT(hashent->pfn == pfn);
        ASSERT(hashent->refcnt != 0);

        hashent->refcnt--;
    }
    else if ( hashent->refcnt == 0 )
    {
        if ( hashent->idx != MAPHASHENT_NOTINUSE )
        {
            /* /First/, zap the PTE. */
            ASSERT((pte_val(cache->l1tab[hashent->idx])>>PAGE_SHIFT) == hashent->pfn);
            /* /Second/, mark as garbage. */
            set_bit(hashent->idx, cache->garbage);
        }

        /* Add newly-freed mapping to the maphash. */
        hashent->pfn = pfn;
        hashent->idx = idx;
    }
    else
    {
        /* /First/, zap the PTE. */
  //      cache->l1tab[idx] = MK_PTE(0, 0);
		consistent_write(&cache->l1tab[idx], 0);
	
        /* /Second/, mark as garbage. */
        set_bit(idx, cache->garbage);
    }

	cpu_flush_tlb_entry(va);
#if 0
	cpu_flush_cache_all();
#endif
}
#endif
#endif
void mapcache_init(void)
{
    unsigned int i, j;
	unsigned long pgd_idx;

	/* Create Mapping for Mapcache */
	mapcache.table = alloc_xenheap_page();
	clear_page(mapcache.table);

	pgd_idx = PGD_IDX(MAPCACHE_VIRT_START);

	for (i = 0; i < (PAGE_SIZE >> 2); i += 256, pgd_idx++) {
		idle_pgd[pgd_idx] = MK_PDE(va_to_ma(&mapcache.table[i]), PDE_TYPE_COARSE | PDE_DOMAIN_HYPERVISOR);
	}

#if 0
    d->arch.mapcache.l1tab = d->arch.mm_perdomain_pt +
        (PERDOMAIN_MAPPING_MBYTES << (20 - PAGE_SHIFT));
#endif

    spin_lock_init(&mapcache.lock);

    /* Mark all maphash entries as not in use. */
    for ( i = 0; i < MAX_VIRT_CPUS; i++ )
        for ( j = 0; j < MAPHASH_ENTRIES; j++ )
            mapcache.vcpu_maphash[i].hash[j].idx = MAPHASHENT_NOTINUSE;
}

#if 0
#define GLOBALMAP_BITS (IOREMAP_MBYTES << (20 - PAGE_SHIFT))
static unsigned long inuse[BITS_TO_LONGS(GLOBALMAP_BITS)];
static unsigned long garbage[BITS_TO_LONGS(GLOBALMAP_BITS)];
static unsigned int inuse_cursor;
static spinlock_t globalmap_lock = SPIN_LOCK_UNLOCKED;

void *map_domain_page_global(unsigned long pfn)
{
    pde_t *pl2e;
    pte_t *pl1e;
    unsigned int idx, i;
    unsigned long va;

    ASSERT(!in_irq() && local_irq_is_enabled());

    spin_lock(&globalmap_lock);

    idx = find_next_zero_bit(inuse, GLOBALMAP_BITS, inuse_cursor);
    va = IOREMAP_VIRT_START + (idx << PAGE_SHIFT);
    if ( unlikely(va >= FIXADDR_START) )
    {
        /* /First/, clean the garbage map and update the inuse list. */
        for ( i = 0; i < ARRAY_SIZE(garbage); i++ )
        {
            unsigned long x = xchg(&garbage[i], 0);
            inuse[i] &= ~x;
        }

        /* /Second/, flush all TLBs to get rid of stale garbage mappings. */
        flush_tlb_all();

        idx = find_first_zero_bit(inuse, GLOBALMAP_BITS);
        va = IOREMAP_VIRT_START + (idx << PAGE_SHIFT);
        ASSERT(va < FIXADDR_START);
    }

    set_bit(idx, inuse);
    inuse_cursor = idx + 1;

    spin_unlock(&globalmap_lock);

    pl2e = virt_to_xen_l2e(va);
    pl1e = l2e_to_l1e(*pl2e) + l1_table_offset(va);
    *pl1e = l1e_from_pfn(pfn, __PAGE_HYPERVISOR);

    return (void *)va;
}

void unmap_domain_page_global(void *va)
{
    unsigned long __va = (unsigned long)va;
    pde_t *pl2e;
    pte_t *pl1e;
    unsigned int idx;

    /* /First/, we zap the PTE. */
    pl2e = virt_to_xen_l2e(__va);
    pl1e = l2e_to_l1e(*pl2e) + l1_table_offset(__va);
    *pl1e = l1e_empty();

    /* /Second/, we add to the garbage map. */
    idx = (__va - IOREMAP_VIRT_START) >> PAGE_SHIFT;
    set_bit(idx, garbage);
}

#endif

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
