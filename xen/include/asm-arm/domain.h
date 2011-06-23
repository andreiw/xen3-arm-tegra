
#ifndef __ARM_DOMAIN_H__
#define __ARM_DOMAIN_H__

#include <asm/mm.h>
#include <asm/page.h>
#include <asm/current.h>
#include <xen/cpumask.h>
#include <xen/cache.h>
#include <xen/spinlock.h>
#include <public/arch-arm.h>

#define TRAP_TABLE_ENTRIES			8

#define TRAP_RESET					0
#define TRAP_UNDEFINED_INSTRUCTION	1
#define TRAP_SOFTWARE_INTERRUPT		2
#define TRAP_PREFETCH_ABORT			3
#define TRAP_DATA_ABORT				4
#define TRAP_RESERVED				5
#define TRAP_INTERRUPT_REQUEST		6
#define TRAP_FAST_INTERRUPT_REQUEST	7

#define MAPHASH_ENTRIES				8
#define MAPHASH_HASHFN(pfn)			((pfn) & (MAPHASH_ENTRIES-1))
#define MAPHASHENT_NOTINUSE			((u16)~0U)

struct vcpu_maphash {
    struct vcpu_maphash_entry {
        unsigned long pfn;
        uint16_t      idx;
        uint16_t      refcnt;
    } hash[MAPHASH_ENTRIES];
}__cacheline_aligned;


#define MAPCACHE_ORDER   10
#define MAPCACHE_ENTRIES (1 << MAPCACHE_ORDER)

struct mapcache {
    /* The PTEs that provide the mappings, and a cursor into the array. */
    pte_t		*table;
    unsigned int cursor;

    /* Protects map_domain_page(). */
    spinlock_t lock;

    /* Which mappings are in use, and which are garbage to reap next epoch? */
    unsigned long inuse[BITS_TO_LONGS(MAPCACHE_ENTRIES)];
    unsigned long garbage[BITS_TO_LONGS(MAPCACHE_ENTRIES)];

    /* Lock-free per-VCPU hash of recently-used mappings. */
    struct vcpu_maphash vcpu_maphash[MAX_VIRT_CPUS];
}__cacheline_aligned;

struct arch_domain
{
    /* I/O-port admin-specified access capabilities. */
    struct rangeset		*ioport_caps;

} __cacheline_aligned;

struct arch_vcpu
{
    unsigned long				flags;
	unsigned long				guest_table;           /* (MA) guest notion of cr3 */
	unsigned long				*guest_vtable;         /* virtual address of pagetable */
    unsigned long				guest_pstart;		/* guest OS physical start address */
    unsigned long				guest_vstart;		/* guest OS virtual start address */
	unsigned long				trap_table[TRAP_TABLE_ENTRIES];
    struct vcpu_guest_context	guest_context;
} __cacheline_aligned;

void startup_cpu_idle_loop(void);

void mapcache_init(void);
#endif 

