#ifndef ASM_ARCH_BARRIERS_H
#define ASM_ARCH_BARRIERS_H

/* TBD. */
#define outer_sync()

#define rmb()		asm("dmb");
#define wmb()		do { asm("dsb"); outer_sync(); } while (0)
#define mb()		wmb()

#endif /* ASM_ARCH_BARRIERS_H */
