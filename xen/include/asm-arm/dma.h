#ifndef __ARCH_DMA_H__
#define __ARCH_DMA_H__
#if 0
#include <asm/arch/dma.h>
#endif
typedef struct dma_channel_info {
	domid_t	owner;
	int	in_use;
}dma_channel_info_t;

extern dma_channel_info_t dma_channel_map[];

#endif /* _ARCH_DMA_H */
