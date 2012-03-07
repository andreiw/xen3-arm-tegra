/*
 * system.c 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          ChanJu Park  <bestworld@samsung.com>
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

#include <xen/config.h>
#include <xen/debug.h>
#include <xen/lib.h>
#include <xen/errno.h>
#include <xen/sched.h>
#include <xen/mm.h>
#include <xen/compile.h>
#include <xen/console.h>
#include <xen/serial.h>
#include <xen/string.h>
#include <xen/xmalloc.h>
#include <xen/linkage.h>
#include <asm/memmap.h>
#include <asm/page.h>
#include <asm/tlbflush.h>
#include <asm/flushtlb.h>
#include <asm/irq.h>
#include <asm/machine.h>
#include <asm/arch/uart.h>

struct ns16550_defaults imx21ads_uart_com1 = {
	.baud      = BAUD_AUTO,
	.data_bits = 8,
	.parity    = 'n',
	.stop_bits = 1
};

struct ns16550_defaults imx21ads_uart_com2 = {
	.baud      = BAUD_AUTO,
	.data_bits = 8,
	.parity    = 'n',
	.stop_bits = 1
};

void imx21ads_sys_clk_init(void)
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
        MPCTL0 = (MPCTL0_PD(0) | MPCTL0_MFD(123) | MPCTL0_MFI(7) | MPCTL0_MFN(115));

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

        /* Restart the SPLL and wait for the CSCR_SPLL_RESTART bit to clear */
        CSCR |= CSCR_SPLL_RESTART;
        while (CSCR & CSCR_SPLL_RESTART);

        /* Set the peripheral clock divider 1 (PERDIV1) to divide by 8 */
        PCDR1 &= ~PCDR1_PERDIV3_MASK;
        PCDR1 |= PCDR1_PERDIV3(5);
}

void init_machine(void)
{
	imx21ads_sys_clk_init();
	imx21ads_uart_init(0, &imx21ads_uart_com1);	
}

