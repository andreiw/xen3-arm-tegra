#include <xen/linkage.h>
#include <asm/asm-macros.h>

	.text
	.align	5
ENTRY(memchr)
1:	subs	r2, r2, #1
	bmi	2f
	ldrb	r3, [r0], #1
	teq	r3, r1
	bne	1b
	sub	r0, r0, #1
2:	movne	r0, #0
	RETINSTR(mov,pc,lr)
