#ifndef _IMX_REGS_H
#define _IMX_REGS_H
/* ------------------------------------------------------------------------
 *  Motorola IMX system registers
 * ------------------------------------------------------------------------
 *
 */

#include <asm/arch/hardware.h>

#define TIMER_BASE IMX_TIM1_BASE

/*
 *  Register BASEs, based on OFFSETs
 *
 */
#define IMX_AIPI1_BASE             (0x00000 + IMX_IO_BASE)
#define IMX_DMAC_BASE              (0x01000 + IMX_IO_BASE)
#define IMX_WDT_BASE               (0x02000 + IMX_IO_BASE)
#define IMX_TIM1_BASE              (0x03000 + IMX_IO_BASE)
#define IMX_TIM2_BASE              (0x04000 + IMX_IO_BASE)
#define IMX_TIM3_BASE              (0x05000 + IMX_IO_BASE)
#define IMX_PWM_BASE               (0x06000 + IMX_IO_BASE)
#define IMX_RTC_BASE               (0x07000 + IMX_IO_BASE)
#define IMX_KPP_BASE               (0x08000 + IMX_IO_BASE)
#define IMX_OWIRE_BASE             (0x09000 + IMX_IO_BASE)
#define IMX_UART1_BASE             (0x0A000 + IMX_IO_BASE)
#define IMX_UART2_BASE             (0x0B000 + IMX_IO_BASE)
#define IMX_UART3_BASE             (0x0C000 + IMX_IO_BASE)
#define IMX_UART4_BASE             (0x0D000 + IMX_IO_BASE)
#define IMX_CSPI1_BASE             (0x0E000 + IMX_IO_BASE)
#define IMX_CSPI2_BASE             (0x0F000 + IMX_IO_BASE)
#define IMX_SSI1_BASE              (0x10000 + IMX_IO_BASE)
#define IMX_SSI2_BASE              (0x11000 + IMX_IO_BASE)
#define IMX_I2C_BASE               (0x12000 + IMX_IO_BASE)
#define IMX_SDHC1_BASE             (0x13000 + IMX_IO_BASE)
#define IMX_SDHC2_BASE             (0x14000 + IMX_IO_BASE)
#define IMX_GPIO_BASE              (0x15000 + IMX_IO_BASE)
#define IMX_AUDMUX_BASE            (0x16000 + IMX_IO_BASE)
#define IMX_CSPI3_BASE             (0x17000 + IMX_IO_BASE)
#define IMX_AIPI2_BASE             (0x20000 + IMX_IO_BASE)
#define IMX_LCDC_BASE              (0x21000 + IMX_IO_BASE)
#define IMX_SLCDC_BASE             (0x22000 + IMX_IO_BASE)
#define IMX_SAHARA_BASE            (0x23000 + IMX_IO_BASE)
#define IMX_USBOTG_BASE            (0x24000 + IMX_IO_BASE)
#define IMX_EMMA_BASE              (0x26000 + IMX_IO_BASE)
#define IMX_CRM_BASE               (0x27000 + IMX_IO_BASE)
#define IMX_FIRI_BASE              (0x28000 + IMX_IO_BASE)
#define IMX_RNGA_BASE              (0x29000 + IMX_IO_BASE)
#define IMX_RTIC_BASE              (0x2A000 + IMX_IO_BASE)
#define IMX_JAM_BASE               (0x3E000 + IMX_IO_BASE)
#define IMX_MAX_BASE               (0x3F000 + IMX_IO_BASE)
#define IMX_AITC_BASE              (0x40000 + IMX_IO_BASE)

#define CSCR                    __REG(IMX_CRM_BASE + 0x0)
#define CSCR_PRESC_MASK         (0x7<<29)
#define CSCR_PRESC(x)           (((x) & 0x7) << 29)
#define CSCR_USB_DIV_MASK       (0x7<<26)
#define CSCR_USB_DIV(x)         (((x) & 0x7) << 26)
#define CSCR_SD_CNT_MASK        (0x3<<24)
#define CSCR_SD_CNT(x)          (((x) & 0x3) << 24)
#define CSCR_SPLL_RESTART       (1<<22)
#define CSCR_MPLL_RESTART       (1<<21)
#define CSCR_SSI2_SEL           (1<<20)
#define CSCR_SSI1_SEL           (1<<19)
#define CSCR_FIR_SEL            (1<<18)
#define CSCR_SP_SEL             (1<<17)
#define CSCR_MCU_SEL            (1<<16)
#define CSCR_BCLKDIV_MASK       (0xf<<10)
#define CSCR_BCLKDIV(x)         (((x) & 0xf) << 10)
#define CSCR_IPDIV              (1<<9)
#define CSCR_OSC26M_DIV1P5      (1<<4)
#define CSCR_OSC28M_DIS         (1<<3)
#define CSCR_FPM_EN             (1<<2)
#define CSCR_SPEN               (1<<1)
#define CSCR_MPEN               (1<<0)
  
#define MPCTL0                  __REG(IMX_CRM_BASE + 0x4)
#define MPCTL0_CPLM             (1<<31)
#define MPCTL0_PD(x)            (((x) & 0xf) << 26)
#define MPCTL0_MFD(x)           (((x) & 0x3ff) << 16)
#define MPCTL0_MFI(x)           (((x) & 0xf) << 10)
#define MPCTL0_MFN(x)           (((x) & 0x3ff) << 0)
  
#define MPCTL1                   __REG(IMX_CRM_BASE + 0x8)
#define MPCTL1_LF               (1<<15)
#define MPCTL1_BRMO             (1<<6)
  
#define SPCTL0                  __REG(IMX_CRM_BASE + 0xc)
#define SPCTL0_CPLM             (1<<31)
#define SPCTL0_PD(x)            (((x) & 0xf) << 26)
#define SPCTL0_MFD(x)           (((x) & 0x3ff) << 16)
#define SPCTL0_MFI(x)           (((x) & 0xf) << 10)
#define SPCTL0_MFN(x)           (((x) & 0x3ff) << 0)
  
#define SPCTL1                  __REG(IMX_CRM_BASE + 0x10)
#define SPCTL1_LF               (1<<15)
#define SPCTL1_BRMO             (1<<6)
#define OSC26MCTL               __REG(IMX_CRM_BASE + 0x14)
#define OSC26MCTL_OSC26M_PEAK   (0x2<<16)
#define OSC25MCTL_AGC(x)        (((x) & 0x3f) << 8)
  
#define PCDR0                   __REG(IMX_CRM_BASE + 0x18)
#define PCDR0_SSI2DIV(x)        (((x) & 0x3f) << 26)
#define PCDR0_SSI2DIV_MASK      (0x3f << 26)
#define PCDR0_SSI1DIV(x)        (((x) & 0x3f) << 16)
#define PCDR0_SSI1DIV_MASK      (0x3f << 16)
#define PCDR0_NFCDIV(x)         (((x) & 0xf) << 12)
#define PCDR0_NFCDIV_MASK       (0xf << 12)
#define PCDR0_CLKO_48MDIV(x)    (((x) & 0x7) << 5)
#define PCDR0_FIRI_DIV(x)       (((x) & 0x1f) << 0)
                 
#define PCDR1                   __REG(IMX_CRM_BASE + 0x1c)
#define PCDR1_PERDIV4_POS       24
#define PCDR1_PERDIV4_MASK      (0x3f << PCDR1_PERDIV4_POS)
#define PCDR1_PERDIV4(x)        (((x) << PCDR1_PERDIV4_POS) & \
                                  PCDR1_PERDIV4_MASK)
#define PCDR1_PERDIV3_POS       16
#define PCDR1_PERDIV3_MASK      (0x3f << PCDR1_PERDIV3_POS)
#define PCDR1_PERDIV3(x)        (((x) << PCDR1_PERDIV3_POS) & \
                                  PCDR1_PERDIV3_MASK)
#define PCDR1_PERDIV2_POS       8
#define PCDR1_PERDIV2_MASK      (0x3f << PCDR1_PERDIV2_POS)
#define PCDR1_PERDIV2(x)        (((x) << PCDR1_PERDIV2_POS) & \
                                  PCDR1_PERDIV2_MASK)
#define PCDR1_PERDIV1_POS       0
#define PCDR1_PERDIV1_MASK      (0x3f << PCDR1_PERDIV1_POS)
#define PCDR1_PERDIV1(x)        (((x) << PCDR1_PERDIV1_POS) & \
                                  PCDR1_PERDIV1_MASK)
  
#define PCCR0                   __REG(IMX_CRM_BASE + 0x20) 
#define PCCR0_HCLK_CSI_EN       (1<<31)
#define PCCR0_HCLK_DMA_EN       (1<<30)
#define PCCR0_HCLK_BROM_EN      (1<<28)
#define PCCR0_HCLK_EMMA_EN      (1<<27)
#define PCCR0_HCLK_LCDC_EN      (1<<26)
#define PCCR0_HCLK_SLCDC_EN     (1<<25)
#define PCCR0_HCLK_USBOTG_EN    (1<<24)
#define PCCR0_HCLK_BMI_EN       (1<<23)
#define PCCR0_PERCLK4_EN        (1<<22)
#define PCCR0_SLCDC_EN          (1<<21)
#define PCCR0_FIRI_BAUD_EN      (1<<20)
#define PCCR0_NFC_EN            (1<<19)
#define PCCR0_PERCLK3_EN        (1<<18)
#define PCCR0_SSI1_BAUD_EN      (1<<17)
#define PCCR0_SSI2_BAUD_EN      (1<<16)
#define PCCR0_EMMA_EN           (1<<15)
#define PCCR0_USBOTG_EN         (1<<14)
#define PCCR0_DMA_EN            (1<<13)
#define PCCR0_I2C_EN            (1<<12)
#define PCCR0_GPIO_EN           (1<<11)
#define PCCR0_SDHC2_EN          (1<<10)
#define PCCR0_SDHC1_EN          (1<<9)
#define PCCR0_FIRI_EN           (1<<8)
#define PCCR0_SSI2_EN           (1<<7)
#define PCCR0_SSI1_EN           (1<<6)
#define PCCR0_CSPI2_EN          (1<<5)
#define PCCR0_CSPI1_EN          (1<<4)
#define PCCR0_UART4_EN          (1<<3)
#define PCCR0_UART3_EN          (1<<2)
#define PCCR0_UART2_EN          (1<<1)
#define PCCR0_UART1_EN          (1<<0)
  
#define PCCR1                   __REG(IMX_CRM_BASE + 0x24)
#define PCCR1_OWIRE_EN          (1<<31)
#define PCCR1_KPP_EN            (1<<30)
#define PCCR1_RTC_EN            (1<<29)
#define PCCR1_PWM_EN            (1<<28)
#define PCCR1_GPT3_EN           (1<<27)
#define PCCR1_GPT2_EN           (1<<26)
#define PCCR1_GPT1_EN           (1<<25)
#define PCCR1_WDT_EN            (1<<24)
#define PCCR1_CSPI3_EN          (1<<23)
#define PCCR1_RTIC_EN           (1<<22)
#define PCCR1_RNGA_EN           (1<<21)
  
#define CCSR                    __REG(IMX_CRM_BASE + 0x28)
#define CCSR_32K_SR             (1<<15)
#define CCSR_CLK0_SEL(x)        (((x) & 0x1f) << 0)

#define WKGDCTL                 __REG(IMX_CRM_BASE + 0x34)
#define WKGDCTL_WKDG_EN         (1<<0)


/*
 *  GPIO Module and I/O Multiplexer
 *  x = 0..3 for reg_A, reg_B, reg_C, reg_D, reg_E, reg_F
 */
#define DDIR(x)    __REG2(IMX_GPIO_BASE + 0x00, ((x) & 7) << 8)
#define OCR1(x)    __REG2(IMX_GPIO_BASE + 0x04, ((x) & 7) << 8)
#define OCR2(x)    __REG2(IMX_GPIO_BASE + 0x08, ((x) & 7) << 8)
#define ICONFA1(x) __REG2(IMX_GPIO_BASE + 0x0c, ((x) & 7) << 8)
#define ICONFA2(x) __REG2(IMX_GPIO_BASE + 0x10, ((x) & 7) << 8)
#define ICONFB1(x) __REG2(IMX_GPIO_BASE + 0x14, ((x) & 7) << 8)
#define ICONFB2(x) __REG2(IMX_GPIO_BASE + 0x18, ((x) & 7) << 8)
#define DR(x)      __REG2(IMX_GPIO_BASE + 0x1c, ((x) & 7) << 8)
#define GIUS(x)    __REG2(IMX_GPIO_BASE + 0x20, ((x) & 7) << 8)
#define SSR(x)     __REG2(IMX_GPIO_BASE + 0x24, ((x) & 7) << 8)
#define ICR1(x)    __REG2(IMX_GPIO_BASE + 0x28, ((x) & 7) << 8)
#define ICR2(x)    __REG2(IMX_GPIO_BASE + 0x2c, ((x) & 7) << 8)
#define IMR(x)     __REG2(IMX_GPIO_BASE + 0x30, ((x) & 7) << 8) 
#define ISR(x)     __REG2(IMX_GPIO_BASE + 0x34, ((x) & 7) << 8)
#define GPR(x)     __REG2(IMX_GPIO_BASE + 0x38, ((x) & 7) << 8)
#define SWR(x)     __REG2(IMX_GPIO_BASE + 0x3c, ((x) & 7) << 8)
#define PUEN(x)    __REG2(IMX_GPIO_BASE + 0x40, ((x) & 7) << 8)
#define PMASK      __REG(IMX_GPIO_BASE + 0x600)


#define GPIO_MODE_OUT   (1<<7)
#define GPIO_MODE_IN    (0<<7)
#define GPIO_MODE_PUEN  (1<<8)

#define GPIO_MODE_PF    (0<<9)
#define GPIO_MODE_AF    (1<<9)

#define GPIO_MODE_OCR_SHIFT 10
#define GPIO_MODE_OCR_MASK (3<<10)
#define GPIO_MODE_AIN   (0<<10)
#define GPIO_MODE_BIN   (1<<10)
#define GPIO_MODE_CIN   (2<<10)
#define GPIO_MODE_DR    (3<<10)

#define GPIO_MODE_AOUT_SHIFT 12
#define GPIO_MODE_AOUT_MASK (3<<12)
#define GPIO_MODE_AOUT     (0<<12)
#define GPIO_MODE_AOUT_ISR (1<<12)
#define GPIO_MODE_AOUT_0   (2<<12)
#define GPIO_MODE_AOUT_1   (3<<12)

#define GPIO_MODE_BOUT_SHIFT 14
#define GPIO_MODE_BOUT_MASK (3<<14)
#define GPIO_MODE_BOUT      (0<<14)
#define GPIO_MODE_BOUT_ISR  (1<<14)
#define GPIO_MODE_BOUT_0    (2<<14)
#define GPIO_MODE_BOUT_1    (3<<14)
#define GPIO_MODE_GIUS      (1<<16)

#define GPIO_PORTA              (0<<5)
#define GPIO_PORTB              (1<<5)
#define GPIO_PORTC              (2<<5)
#define GPIO_PORTD              (3<<5)
#define GPIO_PORTE              (4<<5)
#define GPIO_PORTF              (5<<5)


#if 0
#define GPIO_PORTA(bit) (0+(bit))
#define GPIO_PORTB(bit) (32*1 + (bit))
#define GPIO_PORTC(bit) (32*2 + (bit))
#define GPIO_PORTD(bit) (32*3 + (bit))
#define GPIO_PORTE(bit) (32*4 + (bit))
#define GPIO_PORTF(bit) (32*5 + (bit))
#endif

#if 0
/*
 *  GPIO Mode
 *
 *  The pin, port, data direction, pull-up enable, primary/alternate
 *  function, output configuration, and input configuration are encoded in a 
 *  single word as follows.
 *
 *  4:0 Pin (31-0)
 *  7:5 Port (F-A)
 *  8 Direction
 *  9 PUEN
 *  10:11 Primary Function,Alternate Function
 *  13:12 OCR
 *  15:14 ICONF
 * 
 *  [ 15 14 | 13 12 | 11 10 | 9  |  8  | 7 6 5 | 4 3 2 1 0 ]
 *  [ ICONF |  OCR  | P/A   | PU | Dir | Port  |    Pin    ]
 */

#define GPIO_PIN_MASK           0x1f
#define GPIO_PORT_POS           5
#define GPIO_PORT_MASK          (0x3 << GPIO_PORT_POS)

#define GPIO_PORTA              (0<<GPIO_PORT_POS)
#define GPIO_PORTB              (1<<GPIO_PORT_POS)
#define GPIO_PORTC              (2<<GPIO_PORT_POS)
#define GPIO_PORTD              (3<<GPIO_PORT_POS)
#define GPIO_PORTE              (4<<GPIO_PORT_POS)
#define GPIO_PORTF              (5<<GPIO_PORT_POS)

#define GPIO_DIR_MASK           (1<<8)
#define GPIO_IN                 (0<<8)
#define GPIO_OUT                (1<<8)

#define GPIO_PU_MASK            (1<<9)
#define GPIO_PUDIS              (0<<9)
#define GPIO_PUEN               (1<<9)

#define GPIO_FUNC_MASK          (0x3<<10)
#define GPIO_PF                 (0<<10)
#define GPIO_AF                 (1<<10)

#define GPIO_OCR_POS            12
#define GPIO_OCR_MASK           (3<<GPIO_OCR_POS)
#define GPIO_AIN                (0<<GPIO_OCR_POS)
#define GPIO_BIN                (1<<GPIO_OCR_POS)
#define GPIO_CIN                (2<<GPIO_OCR_POS)
#define GPIO_GPIO               (3<<GPIO_OCR_POS)

#define GPIO_ICONF_MASK         (0x3<<14)
#define GPIO_AOUT               (1<<14)
#define GPIO_BOUT               (2<<14)

/*
 *  The GPIO pin naming convention was developed by the original 
 *  unknown author.  Although using defines for variables is always 
 *  a good idea for portability,  in this case the names are as specific 
 *  as the values, and thus lose their portability. Ultimately the pin 
 *  names should be changed to reflect the signal name only.  
 *
 *  The current naming convention is as follows.
 *
 *  P(port)(pin)_(function)_(signal)
 *
 *  port = (A-F)
 *  pin = (0-31)
 *  function = (PF|AF|AIN|BIN|CIN|DR|AOUT|BOUT)
 *  signal = signal name from datasheet
 *
 *  Remember that when in GPIO mode, AIN, BIN, CIN, and DR are inputs to 
 *  the GPIO peripheral module and represent outputs to the pin. 
 *  Similarly AOUT, and BOUT are outputs from the GPIO peripheral 
 *  module and represent inputs to the physical  pin in question. 
 *  Refer to the multiplexing table in the section titled "Signal 
 *  Descriptions and Pin Assignments" in the reference manual.
 */


/* FIXME: This list is not completed. The correct directions are
 * missing on some (many) pins
 */

#define PE14_PF_UART1_CTS       ( GPIO_PORTE | 14 | GPIO_PF | GPIO_OUT )
#define PE15_PF_UART1_RTS       ( GPIO_PORTE | 15 | GPIO_PF | GPIO_IN )
#define PE12_PF_UART1_TXD       ( GPIO_PORTE | 12 | GPIO_PF | GPIO_OUT )
#define PE13_PF_UART1_RXD       ( GPIO_PORTE | 13 | GPIO_PF | GPIO_IN )

#define PA5_PF_LSCLK            ( GPIO_PORTA | 5 | GPIO_PF | GPIO_OUT )
#define PA6_PF_LD0              ( GPIO_PORTA | 6 | GPIO_PF | GPIO_OUT )
#define PA7_PF_LD1              ( GPIO_PORTA | 7 | GPIO_PF | GPIO_OUT )
#define PA8_PF_LD2              ( GPIO_PORTA | 8 | GPIO_PF | GPIO_OUT )
#define PA9_PF_LD3              ( GPIO_PORTA | 9 | GPIO_PF | GPIO_OUT )
#define PA10_PF_LD4             ( GPIO_PORTA | 10 | GPIO_PF | GPIO_OUT )
#define PA11_PF_LD5             ( GPIO_PORTA | 11 | GPIO_PF | GPIO_OUT )
#define PA12_PF_LD6             ( GPIO_PORTA | 12 | GPIO_PF | GPIO_OUT )
#define PA13_PF_LD7             ( GPIO_PORTA | 13 | GPIO_PF | GPIO_OUT )
#define PA14_PF_LD8             ( GPIO_PORTA | 14 | GPIO_PF | GPIO_OUT )
#define PA15_PF_LD9             ( GPIO_PORTA | 15 | GPIO_PF | GPIO_OUT )
#define PA16_PF_LD10            ( GPIO_PORTA | 16 | GPIO_PF | GPIO_OUT )
#define PA17_PF_LD11            ( GPIO_PORTA | 17 | GPIO_PF | GPIO_OUT )
#define PA18_PF_LD12            ( GPIO_PORTA | 18 | GPIO_PF | GPIO_OUT )
#define PA19_PF_LD13            ( GPIO_PORTA | 19 | GPIO_PF | GPIO_OUT )
#define PA20_PF_LD14            ( GPIO_PORTA | 20 | GPIO_PF | GPIO_OUT )
#define PA21_PF_LD15            ( GPIO_PORTA | 21 | GPIO_PF | GPIO_OUT )
#define PA22_PF_LD16            ( GPIO_PORTA | 22 | GPIO_PF | GPIO_OUT )
#define PA23_PF_LD17            ( GPIO_PORTA | 23 | GPIO_PF | GPIO_OUT )
#define PA24_PF_REV             ( GPIO_PORTA | 24 | GPIO_PF | GPIO_OUT )
#define PA25_PF_CLS             ( GPIO_PORTA | 25 | GPIO_PF | GPIO_OUT )
#define PA26_PF_PS              ( GPIO_PORTA | 26 | GPIO_PF | GPIO_OUT )
#define PA27_PF_SPL_SPR         ( GPIO_PORTA | 27 | GPIO_PF | GPIO_OUT )
#define PA28_PF_LP_HSYNC        ( GPIO_PORTA | 28 | GPIO_PF | GPIO_OUT )
#define PA29_PF_FLM_VSYNC       ( GPIO_PORTA | 29 | GPIO_PF | GPIO_OUT )
#define PA30_PF_CONTRAST        ( GPIO_PORTA | 30 | GPIO_PF | GPIO_OUT )
#define PA31_PF_ACD_OE          ( GPIO_PORTA | 31 | GPIO_PF | GPIO_OUT )
#define PE18_PF_SD_DAT0   ( GPIO_PORTE | GPIO_OUT | GPIO_PF | GPIO_PUEN | 18 )
#define PE19_PF_SD_DAT1   ( GPIO_PORTE | GPIO_OUT | GPIO_PF | GPIO_PUEN | 19 )
#define PE20_PF_SD_DAT2   ( GPIO_PORTE | GPIO_OUT | GPIO_PF | GPIO_PUEN | 20 )
#define PE21_PF_SD_DAT3   ( GPIO_PORTE | GPIO_OUT | GPIO_PF | GPIO_PUEN | 21 )
#define PE23_PF_SD_CLK    ( GPIO_PORTE | GPIO_OUT | GPIO_PF | 23 )
#define PE22_PF_SD_CMD    ( GPIO_PORTE | GPIO_OUT | GPIO_PF | GPIO_PUEN | 22 )
#endif

/*
 *  Watchdog Timer
 */
#define WCR         __REG16(IMX_WDT_BASE + 0x00)
#define WSR         __REG16(IMX_WDT_BASE + 0x02)
#define WRSR        __REG16(IMX_WDT_BASE + 0x04)
#define WCR_WT(x)   (((x) & 0xff) << 8)
#define WCR_WT_MASK (0xff << 8)
#define WCR_WDA     (1 << 5)
#define WCR_SRS     (1 << 4)
#define WCR_WRE     (1 << 3)
#define WCR_WDE     (1 << 2)
#define WCR_WDBG    (1 << 1)
#define WCR_WDZST   (1 << 0)
#define WRSR_PWR    (1 << 4)
#define WRSR_EXT    (1 << 3)
#define WRSR_TOUT   (1 << 1)
#define WRSR_SFTW   (1 << 0)

/*
 *  DMA Controller
 */
#define DCR       __REG(IMX_DMAC_BASE +0x00)	/* DMA Control Register */
#define DISR      __REG(IMX_DMAC_BASE +0x04)	/* DMA Interrupt status Register */
#define DIMR      __REG(IMX_DMAC_BASE +0x08)	/* DMA Interrupt mask Register */
#define DBTOSR    __REG(IMX_DMAC_BASE +0x0c)	/* DMA Burst timeout status Register */
#define DRTOSR    __REG(IMX_DMAC_BASE +0x10)	/* DMA Request timeout Register */
#define DSESR     __REG(IMX_DMAC_BASE +0x14)	/* DMA Transfer Error Status Register */
#define DBOSR     __REG(IMX_DMAC_BASE +0x18)	/* DMA Buffer overflow status Register */
#define DBTOCR    __REG(IMX_DMAC_BASE +0x1c)	/* DMA Burst timeout control Register */
#define WSRA      __REG(IMX_DMAC_BASE +0x40)	/* W-Size Register A */
#define XSRA      __REG(IMX_DMAC_BASE +0x44)	/* X-Size Register A */
#define YSRA      __REG(IMX_DMAC_BASE +0x48)	/* Y-Size Register A */
#define WSRB      __REG(IMX_DMAC_BASE +0x4c)	/* W-Size Register B */
#define XSRB      __REG(IMX_DMAC_BASE +0x50)	/* X-Size Register B */
#define YSRB      __REG(IMX_DMAC_BASE +0x54)	/* Y-Size Register B */
#define SAR(x)    __REG2( IMX_DMAC_BASE + 0x80, (x) << 6)	/* Source Address Registers */
#define DAR(x)    __REG2( IMX_DMAC_BASE + 0x84, (x) << 6)	/* Destination Address Registers */
#define CNTR(x)   __REG2( IMX_DMAC_BASE + 0x88, (x) << 6)	/* Count Registers */
#define CCR(x)    __REG2( IMX_DMAC_BASE + 0x8c, (x) << 6)	/* Control Registers */
#define RSSR(x)   __REG2( IMX_DMAC_BASE + 0x90, (x) << 6)	/* Request source select Registers */
#define BLR(x)    __REG2( IMX_DMAC_BASE + 0x94, (x) << 6)	/* Burst length Registers */
#define RTOR(x)   __REG2( IMX_DMAC_BASE + 0x98, (x) << 6)	/* Request timeout Registers */
#define BUCR(x)   __REG2( IMX_DMAC_BASE + 0x98, (x) << 6)	/* Bus Utilization Registers */
#define CHCNTR(x) __REG2( IMX_DMAC_BASE + 0x9c, (x) << 6)  /* On IMX21 only */
#define CCNR(x)   __REG2( IMX_DMAC_BASE + 0x9C, (x) << 6) 
#define DMA_CH_BASE(x)  (IMX_DMAC_BASE+0x080+0x040*(x))

#define DCR_DAM            (1<<2)  /* On IMX21 only */
#define DCR_DRST           (1<<1)
#define DCR_DEN            (1<<0)
#define DBTOCR_EN          (1<<15)
#define DBTOCR_CNT(x)      ((x) & 0x7fff )
#define CNTR_CNT(x)        ((x) & 0xffffff )
#define CCR_ACRPT          ( 0x1 << 14 )  /* On IMX21 only */
#define CCR_DMOD_LINEAR    ( 0x0 << 12 )
#define CCR_DMOD_2D        ( 0x1 << 12 )
#define CCR_DMOD_FIFO      ( 0x2 << 12 )
#define CCR_DMOD_EOBFIFO   ( 0x3 << 12 )
#define CCR_SMOD_LINEAR    ( 0x0 << 10 )
#define CCR_SMOD_2D        ( 0x1 << 10 )
#define CCR_SMOD_FIFO      ( 0x2 << 10 )
#define CCR_SMOD_EOBFIFO   ( 0x3 << 10 )
#define CCR_MDIR_INC       (0<<9)
#define CCR_MDIR_DEC       (1<<9)
#define CCR_MSEL_A         (0<<8)
#define CCR_MSEL_B         (1<<8)
#define CCR_DSIZ_32        ( 0x0 << 6 )
#define CCR_DSIZ_8         ( 0x1 << 6 )
#define CCR_DSIZ_16        ( 0x2 << 6 )
#define CCR_SSIZ_32        ( 0x0 << 4 )
#define CCR_SSIZ_8         ( 0x1 << 4 )
#define CCR_SSIZ_16        ( 0x2 << 4 )
#define CCR_REN            (1<<3)
#define CCR_RPT            (1<<2)
#define CCR_FRC            (1<<1)
#define CCR_CEN            (1<<0)
#define RTOR_EN            (1<<15)
#define RTOR_CLK           (1<<14)
#define RTOR_PSC           (1<<13)

/*
 *  General purpose timers
 */
#define IMX_TCTL(x)        __REG( 0x00 + (x))
#define TCTL_SWR           (1<<15)
#define TCTL_FRR           (1<<8)
#define TCTL_CAP_RIS       (1<<6)
#define TCTL_CAP_FAL       (2<<6)
#define TCTL_CAP_RIS_FAL   (3<<6)
#define TCTL_OM            (1<<5)
#define TCTL_IRQEN         (1<<4)
#define TCTL_CLK_PCLK1     (1<<1)
#define TCTL_CLK_PCLK1_16  (2<<1)
#define TCTL_CLK_TIN       (3<<1)
#define TCTL_CLK_32        (4<<1)
#define TCTL_TEN           (1<<0)

#define IMX_TPRER(x)       __REG( 0x04 + (x))
#define IMX_TCMP(x)        __REG( 0x08 + (x))
#define IMX_TCR(x)         __REG( 0x0C + (x))
#define IMX_TCN(x)         __REG( 0x10 + (x))
#define IMX_TSTAT(x)       __REG( 0x14 + (x))
#define TSTAT_CAPT         (1<<1)
#define TSTAT_COMP         (1<<0)

/*
 *  Uart controller registers 
 */

#define UART_1  (IMX_UART1_BASE)
#define UART_2  (IMX_UART2_BASE)
#define UART_3  (IMX_UART3_BASE)
#define UART_4  (IMX_UART4_BASE)


#define URXD(x) __REG( 0x0 + (x)) /* Receiver Register */
#define UTXD(x) __REG( 0x40 + (x)) /* Transmitter Register */
#define UCR1(x)  __REG( 0x80 + (x)) /* Control Register 1 */
#define UCR2(x)  __REG( 0x84 + (x)) /* Control Register 2 */
#define UCR3(x)  __REG( 0x88 + (x)) /* Control Register 3 */
#define UCR4(x)  __REG( 0x8c + (x)) /* Control Register 4 */
#define UFCR(x)  __REG( 0x90 + (x)) /* FIFO Control Register */
#define USR1(x)  __REG( 0x94 + (x)) /* Status Register 1 */
#define USR2(x)  __REG( 0x98 + (x)) /* Status Register 2 */
#define UESC(x)  __REG( 0x9c + (x)) /* Escape Character Register */
#define UTIM(x)  __REG( 0xa0 + (x)) /* Escape Timer Register */
#define UBIR(x)  __REG( 0xa4 + (x)) /* BRM Incremental Register */
#define UBMR(x)  __REG( 0xa8 + (x)) /* BRM Modulator Register */
#define UBRC(x)  __REG( 0xac + (x)) /* Baud Rate Count Register */
#define ONMES(x) __REG( 0xb0 + (x)) /* One Millisecond Register */
#define UTS(x)   __REG( 0xb4 + (x)) /* UART Test Register */



/* UART Control Register Bit Fields.*/
#define  URXD_CHARRDY    (1<<15)
#define  URXD_ERR        (1<<14)
#define  URXD_OVRRUN     (1<<13)
#define  URXD_FRMERR     (1<<12)
#define  URXD_BRK        (1<<11)
#define  URXD_PRERR      (1<<10)

#define  UCR1_ADEN       (1<<15) /* Auto dectect interrupt */
#define  UCR1_ADBR       (1<<14) /* Auto detect baud rate */
#define  UCR1_TRDYEN     (1<<13) /* Transmitter ready interrupt enable */
#define  UCR1_IDEN       (1<<12) /* Idle condition interrupt */
#define  UCR1_RRDYEN     (1<<9)  /* Recv ready interrupt enable */
#define  UCR1_RDMAEN     (1<<8)  /* Recv ready DMA enable */
#define  UCR1_IREN       (1<<7)  /* Infrared interface enable */
#define  UCR1_TXMPTYEN   (1<<6)  /* Transimitter empty interrupt enable */
#define  UCR1_RTSDEN     (1<<5)  /* RTS delta interrupt enable */
#define  UCR1_SNDBRK     (1<<4)  /* Send break */
#define  UCR1_TDMAEN     (1<<3)  /* Transmitter ready DMA enable */
// not on mx21 #define  UCR1_UARTCLKEN  (1<<2)   /* UART clock enabled */
#define  UCR1_DOZE       (1<<1)  /* Doze */
#define  UCR1_UARTEN     (1<<0)  /* UART enabled */

#define  UCR2_ESCI       (1<<15) /* Escape seq interrupt enable */
#define  UCR2_IRTS       (1<<14) /* Ignore RTS pin */
#define  UCR2_CTSC       (1<<13) /* CTS pin control */
#define  UCR2_CTS        (1<<12) /* Clear to send */
#define  UCR2_ESCEN      (1<<11) /* Escape enable */
#define  UCR2_PREN       (1<<8)  /* Parity enable */
#define  UCR2_PROE       (1<<7)  /* Parity odd/even */
#define  UCR2_STPB       (1<<6)  /* Stop */
#define  UCR2_WS         (1<<5)  /* Word size */
#define  UCR2_RTSEN      (1<<4)  /* Request to send interrupt enable */
#define  UCR2_ATEN       (1<<3)  /* Aging Timer Enable */
#define  UCR2_TXEN       (1<<2)  /* Transmitter enabled */
#define  UCR2_RXEN       (1<<1)  /* Receiver enabled */
#define  UCR2_SRST       (1<<0)  /* SW reset */

#define  UCR3_DTREN      (1<<13) /* DTR interrupt enable */
#define  UCR3_PARERREN   (1<<12) /* Parity enable */
#define  UCR3_FRAERREN   (1<<11) /* Frame error interrupt enable */
#define  UCR3_DSR        (1<<10) /* Data set ready */
#define  UCR3_DCD        (1<<9)  /* Data carrier detect */
#define  UCR3_RI         (1<<8)  /* Ring indicator */
#define  UCR3_ADNIMP     (1<<7)  /* Timeout interrupt enable */
#define  UCR3_RXDSEN     (1<<6)  /* Receive status interrupt enable */
#define  UCR3_AIRINTEN   (1<<5)  /* Async IR wake interrupt enable */
#define  UCR3_AWAKEN     (1<<4)  /* Async wake interrupt enable */
// not on mx21 #define  UCR3_REF25       (1<<3)  /* Ref freq 25 MHz */
#define  UCR3_RXDMUXSEL  (1<<2)  /* RXD Mux Input Select */
#define  UCR3_INVT       (1<<1)  /* Inverted Infrared transmission */
#define  UCR3_ACIEN      (1<<0)  /* Autobaud Counter  Interrupt Enable */
#define  UCR4_INVR       (1<<9)  /* Inverted infrared reception */
#define  UCR4_ENIRI      (1<<8)  /* Serial infrared interrupt enable */
#define  UCR4_WKEN       (1<<7)  /* Wake interrupt enable */
// not on mx21 #define  UCR4_REF16       (1<<6)  /* Ref freq 16 MHz */
#define  UCR4_IRSC       (1<<5)  /* IR special case */
#define  UCR4_LPBYP      (1<<5)  /* Low Power Bypass */
#define  UCR4_TCEN       (1<<3)  /* Transmit complete interrupt enable */
#define  UCR4_BKEN       (1<<2)  /* Break condition interrupt enable */
#define  UCR4_OREN       (1<<1)  /* Receiver overrun interrupt enable */
#define  UCR4_DREN       (1<<0)  /* Recv data ready interrupt enable */

/* UART_UFCR - UART FIFO Control Register - fields */
#define UFCR_TXTL(x)  (((x) & 0x3F) << 10) /* Transmitter Trigger Level */
#define UFCR_RFDIV(x) (((x) & 0x7) <<  7)  /* Reference Frequency Divider */
#define UFCR_RFDIV_2       (4<<7)
#define UFCR_TXTL_2        (2<<10)
#define UFCR_RXTL_1        (1<<0)
#define UFCR_RFDIV_MASK    (0x7 <<  7)
#define UFCR_RFDIV_SHIFT   (7)

#define UFCR_DCEDTE   (1 <<  6)            /* DCE/DTE Mode Select */
#define UFCR_RXTL(x)  (((x) & 0x3F) <<  0) /* Receiver Trigger Level */


#define  USR1_PARITYERR  (1<<15) /* Parity error interrupt flag */
#define  USR1_RTSS       (1<<14) /* RTS pin status */
#define  USR1_TRDY       (1<<13) /* Transmitter ready interrupt/dma flag */
#define  USR1_RTSD       (1<<12) /* RTS delta */
#define  USR1_ESCF       (1<<11) /* Escape seq interrupt flag */
#define  USR1_FRAMERR    (1<<10) /* Frame error interrupt flag */
#define  USR1_RRDY       (1<<9)  /* Receiver ready interrupt/dma flag */
#define  USR1_AGTIM      (1<<8)  /* Aging Timer Interrupt Flag */
// not on mx21 #define  USR1_TIMEOUT    (1<<7)   /* Receive timeout interrupt status */
#define  USR1_RXDS       (1<<6)  /* Receiver idle interrupt flag */
#define  USR1_AIRINT     (1<<5)  /* Async IR wake interrupt flag */
#define  USR1_AWAKE      (1<<4)  /* Aysnc wake interrupt flag */

#define  USR2_ADET       (1<<15) /* Auto baud rate detect complete */
#define  USR2_TXFE       (1<<14) /* Transmit buffer FIFO empty */
#define  USR2_DTRF       (1<<13) /* DTR edge interrupt flag */
#define  USR2_IDLE       (1<<12) /* Idle condition */
#define  USR2_ACST       (1<<11) /* Autobaud Controller Stopped*/
#define  USR2_RIDELT     (1<<10) /* Ring Indicator Delta */
#define  USR2_RIIN       (1<<9)  /* Ring Indicator Input*/
#define  USR2_IRINT      (1<<8)  /* Serial infrared interrupt flag */
#define  USR2_WAKE       (1<<7)  /* Wake */
#define  USR2_DCDDELT    (1<<6)  /* Data Carrier Delta Detect */
#define  USR2_DCDIN      (1<<5)  /* Data Carrier Detect Input */
#define  USR2_RTSF       (1<<4)  /* RTS edge interrupt flag */
#define  USR2_TXDC       (1<<3)  /* Transmitter complete */
#define  USR2_BRCD       (1<<2)  /* Break condition */
#define  USR2_ORE        (1<<1)  /* Overrun error */
#define  USR2_RDR        (1<<0)  /* Recv data ready */
#define  UTS_FRCPERR     (1<<13) /* Force parity error */
#define  UTS_LOOP        (1<<12) /* Loop tx and rx */
#define  UTS_DBGEN       (1<<11) /* /Debug Enable */
#define  UTS_LOOPIR      (1<<10) /* Loop tx and rx for IR */
#define  UTS_RXFIFO      (1<<9)  /* RXFifo Debug */
#define  UTS_TXEMPTY     (1<<6)  /* TxFIFO empty */
#define  UTS_RXEMPTY     (1<<5)  /* RxFIFO empty */
#define  UTS_TXFULL      (1<<4)  /* TxFIFO full */
#define  UTS_RXFULL      (1<<3)  /* RxFIFO full */
#define  UTS_SOFTRST     (1<<0)  /* Software reset */


#endif				// _IMX_REGS_H
