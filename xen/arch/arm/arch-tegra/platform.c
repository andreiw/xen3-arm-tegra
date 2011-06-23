/*
 * platform.c
 *
 * Copyright (C) 2008 Samsung Electronics 
 *         JaeMin Ryu  <jm77.ryu@samsung.com>
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

#include <xen/spinlock.h>
#include <xen/lib.h>
#include <xen/serial.h>
#include <asm/platform.h>
#include <asm/irq.h>

void imx21_gpio_mode(unsigned int gpio_nr,unsigned int gpio_mode)
{
	uint32_t tmp;
	int port = gpio_nr >> 5;
	int pin = gpio_nr & 0x1f;
	unsigned int ocr = (gpio_mode & GPIO_MODE_OCR_MASK) >> GPIO_MODE_OCR_SHIFT;

	if(gpio_mode & GPIO_MODE_PUEN) {
		PUEN(port) |= (1<<pin);
	} else {
		PUEN(port) &= ~(1<<pin);
	}

	if(gpio_mode & GPIO_MODE_OUT) {
		DDIR(port) |= 1<<pin;
	} else {
		DDIR(port) &= ~(1<<pin);
	}

	/* Primary / alternate function */
	if(gpio_mode & GPIO_MODE_AF) {
		GPR(port) |= (1<<pin);
	} else {
		GPR(port) &= ~(1<<pin);
	}

	/* use as gpio? */
	if(gpio_mode &  GPIO_MODE_GIUS) {
		GIUS(port) |= (1<<pin);
	} else {
		GIUS(port) &= ~(1<<pin);
	}

	if(pin<16) {
		tmp = OCR1(port);
		tmp &= ~( 3<<(pin*2));
		tmp |= (ocr << (pin*2));
		OCR1(port) = tmp;

		ICONFA1(port) &= ~( 3<<(pin*2));
		ICONFA1(port) |= ((gpio_mode >> GPIO_MODE_AOUT_SHIFT) & 3) << (pin * 2);
		ICONFB1(port) &= ~( 3<<(pin*2));
		ICONFB1(port) |= ((gpio_mode >> GPIO_MODE_BOUT_SHIFT) & 3) << (pin * 2);
	} else {
		tmp = OCR2(port);
		tmp &= ~( 3<<((pin-16)*2));
		tmp |= (ocr << ((pin-16)*2));
		OCR2(port) = tmp;

		ICONFA2(port) &= ~( 3<<((pin-16)*2));
		ICONFA2(port) |= ((gpio_mode >> GPIO_MODE_AOUT_SHIFT) & 3) << ((pin-16) * 2);
		ICONFB2(port) &= ~( 3<<((pin-16)*2));
		ICONFB2(port) |= ((gpio_mode >> GPIO_MODE_BOUT_SHIFT) & 3) << ((pin-16) * 2);
	}
}

static void imx21ads_platform_halt(int mode)
{
}

DECLARE_PLATFORM_OP(platform_halt, imx21ads_platform_halt);

static void imx21ads_memory_init(void)
{
	register_memory_bank(0xC0000000, 64 * 1024 * 1024);
}

static void imx21ads_uart_putc(struct serial_port *port, char c)
{
	while(UTS(UART_1) & UTS_TXFULL);

	UTXD(UART_1) = c;

	if(c == '\n') {
		imx21ads_uart_putc(port, '\r');
	}
}

static int imx21ads_uart_getc(struct serial_port *port, char *pc)
{
	return 1;
}

static struct ns16550_defaults imx21ads_uart_params = {
	.baud      = BAUD_AUTO,
	.data_bits = 8,
	.parity    = 'n',
	.stop_bits = 1
};

static struct uart_driver imx21ads_uart_driver = {
	.putc = imx21ads_uart_putc,
	.getc = imx21ads_uart_getc
};

#define RX_MODE	(GPIO_MODE_PF | GPIO_MODE_IN)
#define TX_MODE	(GPIO_MODE_PF | GPIO_MODE_OUT)

static void imx21ads_uart_init(void)
{
	imx21_gpio_mode(GPIO_PORTE + 12, TX_MODE);
	imx21_gpio_mode(GPIO_PORTE + 13, RX_MODE);

	UCR2(UART_1) = 0;
	UCR3(UART_1) |= UCR3_RXDMUXSEL;

	while((USR2(UART_1) & USR2_TXFE) == 0);

	UCR1(UART_1) = UCR1_UARTEN;

	/*
	 *   (Desired Baud Rate * 16)     NUM
	 * --------------------------- = -----
	 *  Reference Frequency (16Mhz)  DENOM
	 *
	 *  UBIR = NUM - 1
	 *  IBMR = DENOM - 1
	 */
	UBIR(UART_1) = 1152 - 1;
	UBMR(UART_1) = 10000 - 1;

	//UFCR(UART_1) = UFCR_TXTL_2;
	UCR2(UART_1) = UCR2_IRTS | UCR2_WS | UCR2_TXEN | UCR2_SRST; 

	serial_register_uart(0, &imx21ads_uart_driver, &imx21ads_uart_params);	
}

static void imx21ads_sys_clk_init(void)
{
        /*
         * System clock initialization
         *
         * The following register settings are similar to the ones currently
         * configured in the Freescale distributions of Redboot and Grub.
         * All common clock settings should go here and specific settings should
         * go in their respective drivers.
         */

        /* Enable the frequency premultiplier (FPM) to multiply by 512 */
        CSCR |= CSCR_FPM_EN;

        /* Select the FPM output as the input to the MPLL and the SPLL */
        CSCR &= ~(CSCR_MCU_SEL | CSCR_SP_SEL);

        /* Enable the MPLL and the SPLL */
        CSCR |= (CSCR_MPEN | CSCR_SPEN);

        /*
         * Set the MPLL so the output frequency is 266MHz by setting
         * PD=0, MFD=123, MFI=7, and MFN=115 when fref=32.768kHz
         */
        //MPCTL0 = (MPCTL0_PD(0) | MPCTL0_MFD(123) | MPCTL0_MFI(7) | MPCTL0_MFN(115));
        /*
         * Set the MPLL so the output frequency is 288MHz by setting
         */
        MPCTL0 = (MPCTL0_PD(0) | MPCTL0_MFD(626) | MPCTL0_MFI(8) | MPCTL0_MFN(365));

        /* Set the prescaler (PRESC) to divide by 1 */
        CSCR &= ~CSCR_PRESC_MASK;
        CSCR |= CSCR_PRESC(0);

        /* Enable the peripheral clock divider (IPDIV) to divide by 2 */
        CSCR |= CSCR_IPDIV;

        /* Enable HCLK input to the BROM, DMA modules */
        PCCR0 |= (PCCR0_HCLK_BROM_EN | PCCR0_HCLK_DMA_EN | PCCR0_DMA_EN);
        /* Restart the MPLL and wait for the CSCR_MPLL_RESTART bit to clear */
        CSCR |= CSCR_MPLL_RESTART;
        while (CSCR & CSCR_MPLL_RESTART)
                ;

        /*
         * Set the SPLL so the output frequency is 288MHz by setting
         * PD=0, MFD=626, MFI=8, and MFN=365 when fref=32.768kHz
         */
        SPCTL0 = (SPCTL0_PD(0) | SPCTL0_MFD(626) | SPCTL0_MFI(8) | SPCTL0_MFN(365));
        //SPCTL0 = (SPCTL0_PD(0) | SPCTL0_MFD(127) | SPCTL0_MFI(8) | SPCTL0_MFN(101));

        /* Restart the SPLL and wait for the CSCR_SPLL_RESTART bit to clear */
        CSCR |= CSCR_SPLL_RESTART;
        while (CSCR & CSCR_SPLL_RESTART);

        /* Set the peripheral clock divider 1 (PERDIV1) to divide by 9 */
	PCDR1 &= ~PCDR1_PERDIV1_MASK;
	//PCDR1 |= PCDR1_PERDIV1(7);
	PCDR1 |= PCDR1_PERDIV1(8);

        /* Set the peripheral clock divider 1 (PERDIV1) to divide by 6 */
        PCDR1 &= ~PCDR1_PERDIV3_MASK;
        PCDR1 |= PCDR1_PERDIV3(5);

	PCDR0 &= ~(PCDR0_SSI1DIV_MASK | PCDR0_SSI2DIV_MASK);
	PCDR0 |= PCDR0_SSI1DIV(22) | PCDR0_SSI2DIV(22);

	PCDR0 &= ~(PCDR0_NFCDIV_MASK);
	PCDR0 |= PCDR0_NFCDIV(9);
}

static void imx21ads_platform_setup(void)
{
	imx21ads_memory_init();
        imx21ads_sys_clk_init();
	imx21ads_uart_init();
        imx21_irq_init();
        imx_timer_init();
}

DECLARE_PLATFORM_OP(platform_setup, imx21ads_platform_setup);

static void imx21ads_platform_query(struct query_data *query)
{
	switch(query->type) {
		case QUERY_MEMORY_DETAILS : break;
		case QUERY_CPU_DETAILS : break;
		default : break;
	};
}

DECLARE_PLATFORM_OP(platform_query, imx21ads_platform_query);
