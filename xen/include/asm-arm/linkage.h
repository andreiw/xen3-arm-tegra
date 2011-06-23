#ifndef __ASM_ARM_LINKAGE_H__
#define __ASM_ARM_LINKAGE_H__

#ifdef __ASSEMBLY__
#define __ALIGN .align 0
#define __ALIGN_STR ".align 0"
#endif

#define HYPERCALL_HANDLER	__attribute__((interrupt("SWI")))
#define INTERRUPT_HANDLER	__attribute__((interrupt("IRQ")))
#define NAKED				__attribute__((naked))
#endif
