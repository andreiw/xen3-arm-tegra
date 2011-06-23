#ifndef __IMX21ADS_DMA_H
#define __IMX21ADS_DMA_H

#include <asm/arch/imx-regs.h>

#define IMX21ADS_DMA_CHANNELS	16
#define MAX_DMA_CHANNELS	IMX21ADS_DMA_CHANNELS


#define DMAC_BASE_ADDR		0x10001000           /* Base Add */

/* Base address for DMA channel registers */
#define DMAC_BASE_CHANNEL_ADDR	0x10001080
/* DMA definitions with variable channel numbers (range: 0 - 15) */
#define DMAC_CHANNEL_MAX	16
#define DMAC_CHANNEL_STRIDE	0x0040

/* DMAC_DCR - DMA Control Register field definitions */
#define DMAC_DCR_DAM            (1 << 2)        /* DMA Access Mode */
#define DMAC_DCR_DRST           (1 << 1)        /* DMA Soft Reset */
#define DMAC_DCR_DEN            (1 << 0)        /* DMA Enable */

/* DMAC_DISR - DMA Interrupt Status Register - bits 0-15 = Channels 1-16 */
#define DMAC_DISR_CH(channel)   (1 << (channel))

/* DMAC_DIMR - DMA Interrupt Mask Register */
#define DMAC_DIMR_CH(channel)   (1 << (channel))

/* DMAC_DBTOSR - DMA Burst Time-Out Status Register */
#define DMAC_DBTOSR_CH(channel) (1 << (channel))

/* DMAC_DRTOSR - DMA Request Time-Out Status Register */
#define DMAC_DRTOSR_CH(channel) (1 << (channel))

/* DMAC_DSESR - DMA Transfer Error Status Register */
#define DMAC_DSESR_CH(channel)  (1 << (channel))

/* DMAC_DBOSR - DMA Buffer Overflow Status Register */
#define DMAC_DBOSR_CH(channel)  (1 << (channel))

/* DMAC_DBTOCR - DMA Burst Time-Out Control Register */
#define DMAC_DBTOCR_EN          (1 << 15)
#define DMAC_DBTOCR_CNT(x)      ((x) & 0x7FFF)

/* DMAC_WSRA WSRB - W-Size Register A W-Size Register B */
#define DMAC_WSRA WSRB_WS(x)    ((x) & 0xFFFF)

/* DMAC_XSRA XSRB - X-Size Register A X-Size Register B */
#define DMAC_XSRA XSRB_XS(x)    ((x) & 0xFFFF)

/* DMAC_YSRA YSRB - Y-Size Register A Y-Size Register B */
#define DMAC_YSRA YSRB_YS(x)    ((x) & 0xFFFF)

/* DMAC Channel Control Register bit field definitions */
#define DMAC_CCR_ACRPT          (1 << 14)       /* Auto Clear RPT */
#define DMAC_CCR_DMOD(x)        (((x)>>12)&0x3) /* Dest Mode- 2D, linear, etc */
#define DMAC_CCR_SMOD(x)        (((x)>>10)&0x3) /* Source Mode */
#define DMAC_CCR_MDIR           (1 << 9)        /* Memory Direction */
#define DMAC_CCR_MSEL           (1 << 8)        /* Memory Select */
#define DMAC_CCR_DSIZ(x)        (((x)>>6)&0x3)  /* Dest Size - 8,16,32 */
#define DMAC_CCR_SSIZ(x)        (((x)>>4)&0x3)  /* Source Size */
#define DMAC_CCR_REN            (1 << 3)        /* Request Enable */
#define DMAC_CCR_RPT            (1 << 2)        /* Repeat */
#define DMAC_CCR_FRC            (1 << 1)        /* Force DMA Cycle */
#define DMAC_CCR_CEN            (1 << 0)        /* Channel Enable */

#define DMAC_CCR_SET_SSIZ(x,value)      ((x)|=(value<<4))
#define DMAC_CCR_SET_DSIZ(x,value)      ((x)|=(value<<6))
#define DMAC_CCR_SET_SMOD(x,value)      ((x)|=(value<<10))
#define DMAC_CCR_SET_DMOD(x,value)      ((x)|=(value<<12))


#define DMA_MEM_SIZE_8  	0x1
#define DMA_MEM_SIZE_16 	0x2
#define DMA_MEM_SIZE_32 	0x0

#define DMA_TYPE_LINEAR 	0x0
#define DMA_TYPE_2D             0x01
#define DMA_TYPE_FIFO   	0x2
#define DMA_TYPE_EBE    	0x3
#define DMA_MEM_SIZE_8          0x1
#define DMA_MEM_SIZE_16         0x2
#define DMA_MEM_SIZE_32         0x0

#define DMA_DONE                0x1000
#define DMA_BURST_TIMEOUT       0x1
#define DMA_REQUEST_TIMEOUT     0x2
#define DMA_TRANSFER_ERROR      0x4
#define DMA_BUFFER_OVERFLOW     0x8

#define DMA_REQ_CSI_RX          31
#define DMA_REQ_CSI_STAT        30
#define DMA_REQ_BMI_RX          29
#define DMA_REQ_BMI_TX          28
#define DMA_REQ_UART1_TX        27
#define DMA_REQ_UART1_RX        26
#define DMA_REQ_UART2_TX        25
#define DMA_REQ_UART2_RX        24
#define DMA_REQ_UART3_TX        23
#define DMA_REQ_UART3_RX        22
#define DMA_REQ_UART4_TX        21
#define DMA_REQ_UART4_RX        20
#define DMA_REQ_CSPI1_TX        19
#define DMA_REQ_CSPI1_RX        18
#define DMA_REQ_CSPI2_TX        17
#define DMA_REQ_CSPI2_RX        16
#define DMA_REQ_SSI1_TX1        15
#define DMA_REQ_SSI1_RX1        14
#define DMA_REQ_SSI1_TX0        13
#define DMA_REQ_SSI1_RX0        12
#define DMA_REQ_SSI2_TX1        11
#define DMA_REQ_SSI2_RX1        10
#define DMA_REQ_SSI2_TX0        9
#define DMA_REQ_SSI2_RX0        8
#define DMA_REQ_SDHC1           7
#define DMA_REQ_SDHC2           6
#define DMA_FIRI_TX             5
#define DMA_FIRI_RX             4
#define DMA_EX                  3
#define DMA_REQ_CSPI3_TX        2
#define DMA_REQ_CSPI3_RX        1

#define DMA_CTL_CEN		0x1
#define DMA_CTL_FRC		0x2
#define DMA_CTL_RPT		0x4
#define DMA_CTL_REN		0x8

#define DMA_CTL_SSIZ		(0x10|0x20)
#define DMA_CTL_DSIZ		(0x40|0x80)

#define DMA_CTL_MSEL		0x100
#define DMA_CTL_MDIR		0x200
#define DMA_CTL_SMOD		(0x400|0x800)
#define DMA_CTL_DMOD		(0x1000|0x2000)
#define DMA_CTL_ACRPT		0x4000

#define DMA_CTL_GET_SSIZ(x)	(((x)>>4)&0x3)
#define DMA_CTL_GET_DSIZ(x)	(((x)>>6)&0x3)
#define DMA_CTL_GET_SMOD(x)	(((x)>>10)&0x3)
#define DMA_CTL_GET_DMOD(x)	(((x)>>12)&0x3)


#define DMA_REQ_CSI_RX		31
#define DMA_REQ_CSI_STAT	30
#define DMA_REQ_BMI_RX		29
#define DMA_REQ_BMI_TX		28
#define DMA_REQ_UART1_TX	27
#define DMA_REQ_UART1_RX	26
#define DMA_REQ_UART2_TX	25
#define DMA_REQ_UART2_RX	24
#define DMA_REQ_UART3_TX	23
#define DMA_REQ_UART3_RX	22
#define DMA_REQ_UART4_TX	21
#define DMA_REQ_UART4_RX	20
#define DMA_REQ_CSPI1_TX	19
#define DMA_REQ_CSPI1_RX	18
#define DMA_REQ_CSPI2_TX	17
#define DMA_REQ_CSPI2_RX	16
#define DMA_REQ_SSI1_TX1	15
#define DMA_REQ_SSI1_RX1	14
#define DMA_REQ_SSI1_TX0	13
#define DMA_REQ_SSI1_RX0	12
#define DMA_REQ_SSI2_TX1	11
#define DMA_REQ_SSI2_RX1	10
#define DMA_REQ_SSI2_TX0	9
#define DMA_REQ_SSI2_RX0	8
#define DMA_REQ_SDHC1		7
#define DMA_REQ_SDHC2		6
#define DMA_FIRI_TX		5
#define DMA_FIRI_RX		4
#define DMA_EX			3


#define DMA_CTL_SET_SSIZ(x,value)		\
	do{					\
		(x)&=~(0x3<<4);			\
                (x)|=(value)<<4;                \
	}while(0)

#define DMA_CTL_SET_DSIZ(x,value)		\
	do{					\
		(x)&=~(0x3<<6); 		\
		(x)|=(value)<<6;                \
	}while(0)

#define DMA_CTL_SET_SMOD(x,value)		\
	do{ 					\
		(x)&=~(0x3<<10); 		\
		(x)|=(value)<<10;               \
	}while(0)

#define DMA_CTL_SET_DMOD(x,value)		\
	do{ 					\
		(x)&=~(0x3<<12);		\
		(x)|=(value)<<12;               \
	}while(0)


typedef enum {
	IMX21ADS_DMA_PRIO_HIGH = 0,
	IMX21ADS_DMA_PRIO_MEDIUM = 3,
	IMX21ADS_DMA_PRIO_LOW = 6
} imx21ads_dma_priority;

static inline void imx21ads_get_dma_status(unsigned long channel, struct dma_status *status)
{
	status->interrupt	= DISR;
	status->request_timeout = DRTOSR;
	status->burst_timeout	= DBTOSR;
	status->transfer_error	= DSESR;
	status->buffer_overflow = DBOSR;
}

static inline void imx21ads_ack_dma_int(unsigned long channel)
{
	DISR    = DMAC_DISR_CH(channel);
	DRTOSR  = DMAC_DRTOSR_CH(channel);
	DBTOSR  = DMAC_DBTOSR_CH(channel);
	DSESR   = DMAC_DSESR_CH(channel);
	DBOSR   = DMAC_DBOSR_CH(channel);
}

static inline void imx21ads_set_dma_config(unsigned long channel, struct dma_config *config)
{

	BLR(channel)  = config->length;
	SAR(channel)  = config->source_address;
	DAR(channel)  = config->destination_address;
	RSSR(channel) = config->request;
	CNTR(channel) = config->count;

        if(config->direction)
		CCR(channel) |=DMAC_CCR_MDIR;
	else
		CCR(channel) &=~DMAC_CCR_MDIR;

	if(config->repeat)
		CCR(channel) |=DMAC_CCR_RPT;
	else
		CCR(channel) &=~DMAC_CCR_RPT;

	DMAC_CCR_SET_SMOD(CCR(channel), config->source_type);
	DMAC_CCR_SET_SSIZ(CCR(channel), config->source_port);
	DMAC_CCR_SET_DMOD(CCR(channel), config->destination_type);
	DMAC_CCR_SET_DSIZ(CCR(channel), config->destination_port);

}

static inline void imx21ads_get_dma_config(unsigned long channel, struct dma_config *config)
{
	config->direction		= CCR(channel)& DMAC_CCR_MDIR ? 1:0;
	config->length			= BLR(channel);
	config->repeat			= CCR(channel) & DMAC_CCR_RPT ? 1:0;
	config->source_type		= DMAC_CCR_SMOD(CCR(channel));
	config->source_address		= SAR(channel);
	config->source_port		= DMAC_CCR_SSIZ(CCR(channel));
	config->destination_type	= DMAC_CCR_DMOD(CCR(channel));
	config->destination_address	= DAR(channel);
	config->destination_port	= DMAC_CCR_DSIZ(CCR(channel));
	config->request			= RSSR(channel);
	config->count			= CNTR(channel);
}

static inline void imx21ads_set_dma_count(unsigned long channel, unsigned long count)
{
	CNTR(channel) = count;
}

static inline void imx21ads_set_dma_address(unsigned long channel, unsigned long addr, unsigned int flag)
{
	if(flag) {
		SAR(channel) = addr;
	} else {
		DAR(channel) = addr;
	}
}

static inline void imx21ads_disable_dma(unsigned long channel)
{
	CCR(channel) &= ~DMAC_CCR_CEN;
}

static inline void imx21ads_start_dma(unsigned long channel)
{
	DISR    = DMAC_DISR_CH(channel);
	DRTOSR  = DMAC_DRTOSR_CH(channel);
	DBTOSR  = DMAC_DBTOSR_CH(channel);
	DSESR   = DMAC_DSESR_CH(channel);
	DBOSR   = DMAC_DBOSR_CH(channel);

	CCR(channel) &= ~DMAC_CCR_CEN;
	CCR(channel) &= ~DMAC_CCR_REN;

	if(DMAC_CCR_SMOD(CCR(channel)) == DMA_TYPE_FIFO) {
		CCR(channel) |= DMAC_CCR_REN;
	}

	if(DMAC_CCR_DMOD(CCR(channel)) == DMA_TYPE_FIFO) {
		CCR(channel) |= DMAC_CCR_REN;
	}


	CCR(channel) |= DMAC_CCR_CEN;	
}


static inline void imx21ads_enable_dma(unsigned long channel)
{
	DISR		= DMAC_DISR_CH(channel);
	DBTOSR		= DMAC_DBTOSR_CH(channel);
	DRTOSR		= DMAC_DRTOSR_CH(channel);
	DSESR		= DMAC_DSESR_CH(channel);
	DBOSR		= DMAC_DBOSR_CH(channel);

	CCR(channel) 	&= ~DMAC_CCR_CEN;
	CCR(channel) 	|= DMAC_CCR_CEN;
}

static inline void imx21ads_get_dma_residue(unsigned long channel, unsigned long *residue)
{
	*residue = (CNTR(channel) - CCNR(channel));
}

static inline void imx21ads_request_dma(unsigned long channel)
{
	DIMR &= ~(DMAC_DIMR_CH(channel));
	DBTOCR = (1 << 15) + 0x500;

	DCR |= DMAC_DCR_DEN;

}

static inline void imx21ads_release_dma(unsigned long channel)
{
	DIMR |= DMAC_DIMR_CH(channel);
}

#define arch_enable_dma		imx21ads_enable_dma
#define arch_disable_dma	imx21ads_disable_dma
#define arch_start_dma		imx21ads_start_dma
#define arch_request_dma	imx21ads_request_dma
#define arch_release_dma	imx21ads_release_dma
#define arch_ack_dma_int	imx21ads_ack_dma_int
#define arch_set_dma_count	imx21ads_set_dma_count
#define arch_set_dma_address	imx21ads_set_dma_address
#define arch_set_dma_config	imx21ads_set_dma_config
#define arch_get_dma_status	imx21ads_get_dma_status
#define arch_get_dma_residue	imx21ads_get_dma_residue
#define arch_get_dma_config	imx21ads_get_dma_config
#endif
