#include <xen/linkage.h>
#include <asm/asm-macros.h>

		.text

/*
 * Purpose  : Function to set a bit
 * Prototype: int set_bit(int bit, void *addr)
 */
ENTRY(_set_bit_be)
		eor	r0, r0, #0x18		@ big endian byte ordering
ENTRY(_set_bit_le)
		and	r2, r0, #7
		mov	r3, #1
		mov	r3, r3, lsl r2
		save_and_disable_irqs ip, r2
		ldrb	r2, [r1, r0, lsr #3]
		orr	r2, r2, r3
		strb	r2, [r1, r0, lsr #3]
		restore_irqs ip
		RETINSTR(mov,pc,lr)
