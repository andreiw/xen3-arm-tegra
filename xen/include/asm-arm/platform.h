#ifndef __ASM_PLATFORM_H__
#define __ASM_PLATFORM_H__

#include <asm/memory.h>

#define QUERY_MEMORY_DETAILS	0x00000000
#define QUERY_CPU_DETAILS	0x00000001
#define QUERY_CACHE_DETAILS	0x00000002
#define QUERY_IRQ_DETAILS	0x00000003
#define QUERY_DMA_DETAILS	0x00000004

#ifdef __ASSEMBLY__
#define DECLARE_PLATFORM_OP(gop, nop)	\
        .set gop, nop                   ;\
	.global gop                     ;
#else
#define DECLARE_PLATFORM_OP(gop, nop)	\
        typeof (nop) gop                \
	__attribute__((weak, alias(#nop)))



struct memory_details {
	unsigned long banks;
	struct memory_bank bank[NR_BANKS];
};

struct query_data {
	unsigned long type;
	union {
		struct memory_details memory_details;
	}data;
};

/*
 * Prototype of platform operations
 */
void platform_setup(void);
void platform_halt(int mode);
void platform_query(struct query_data *query);
#endif
#endif

