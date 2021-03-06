/*
 *  linux/arch/arm/kernel/debug-armv.S
 *
 *  Copyright (C) 1994-1999 Russell King
 *  Copyright (C) 2011 Andrei Warkentin <andreiw@motorola.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  32-bit debugging code
 */

#include <xen/config.h>
#include <asm/arch/hardware.h>
#include <asm/serial_reg.h>
#include <xen/linkage.h>
#include <xen/sizes.h>

#define UART_SHIFT 2
	
	
		.text

                .macro  addruart,rx
                mrc     p15, 0, \rx, c1, c0
                tst     \rx, #1                 @ MMU enabled?
	        ldrne   \rx, =TEGRA_APB_VIRT
	        ldreq   \rx, =TEGRA_APB_PHYS
	        orr     \rx, \rx, #(TEGRA_DEBUG_UART_BASE & 0xFF)
		orr     \rx, \rx, #(TEGRA_DEBUG_UART_BASE & 0xFF00)
                .endm

                .macro  senduart,rd,rx
                strb    \rd, [\rx, #UART_TX << UART_SHIFT]
                .endm

                .macro  busyuart,rd,rx
1002:           ldrb    \rd, [\rx, #UART_LSR << UART_SHIFT]
                and     \rd, \rd, #UART_LSR_TEMT | UART_LSR_THRE
                teq     \rd, #UART_LSR_TEMT | UART_LSR_THRE
                bne     1002b
                .endm

                .macro  waituart,rd,rx
#ifdef FLOW_CONTROL
1001:           ldrb    \rd, [\rx, #UART_MSR << UART_SHIFT]
                tst     \rd, #UART_MSR_CTS
                beq     1001b
#endif
                .endm


/*
 * Useful debugging routines
 */
ENTRY(printhex8)
		mov	r1, #8
		b	printhex

ENTRY(printhex4)
		mov	r1, #4
		b	printhex

ENTRY(printhex2)
		mov	r1, #2
printhex:	adr	r2, hexbuf
		add	r3, r2, r1
		mov	r1, #0
		strb	r1, [r3]
1:		and	r1, r0, #15
		mov	r0, r0, lsr #4
		cmp	r1, #10
		addlt	r1, r1, #'0'
		addge	r1, r1, #'a' - 10
		strb	r1, [r3, #-1]!
		teq	r3, r2
		bne	1b
		mov	r0, r2
		b	printascii

		.ltorg

ENTRY(printascii)
		addruart r3
		b	2f
1:		waituart r2, r3
		senduart r1, r3
		busyuart r2, r3
		teq	r1, #'\n'
		moveq	r1, #'\r'
		beq	1b
2:		teq	r0, #0
		ldrneb	r1, [r0], #1
		teqne	r1, #0
		bne	1b
		mov	pc, lr

ENTRY(printch)
		addruart r3
		mov	r1, r0
		mov	r0, #0
		b	1b

hexbuf:		.space 16
