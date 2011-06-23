#ifndef __ASM_MEMORY_H__
#define __ASM_MEMORY_H__

#include <asm/config.h>

#ifdef ARCH_NR_BANKS
#define NR_BANKS	ARCH_NR_BANKS
#else
#define NR_BANKS	4
#endif

struct memory_bank {
	unsigned long	base;
	unsigned long	size;
	int		node;
};


struct meminfo {
	unsigned long nr_banks;
	struct memory_bank banks[NR_BANKS];
};

extern struct meminfo system_memory;
void register_memory_bank(unsigned long base, unsigned long size);

static inline void consistent_write(void *ptr, unsigned long value)
{
	__asm__ __volatile__(
			"str        %1, [%0]                								\n"
			"nop																\n"
			"bic		%0, %0, #0x1f											\n"
			"mcr        p15, 0, %0, c7, c10, 1		@ clean D entry				\n"
			"mcr        p15, 0, %2, c7, c10, 4  	@ drain WB					\n"
			:
			: "r"(ptr), "r"(value), "r"(0)
			: "memory");
}

#endif

