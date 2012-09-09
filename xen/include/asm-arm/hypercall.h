#ifndef __ASM_ARM_HYPERCALL_H__
#define __ASM_ARM_HYPERCALL_H__

#include <public/physdev.h>

extern long do_set_trap_table(GUEST_HANDLE(trap_info_t) traps);

extern int do_mmu_update(GUEST_HANDLE(mmu_update_t) ureqs,
			 unsigned int count,
			 GUEST_HANDLE(uint) pdone,
			 unsigned int foreigndom);

extern long do_set_gdt(GUEST_HANDLE(ulong) frame_list,
		       unsigned int entries);

extern long do_stack_switch(unsigned long ss, unsigned long esp);

extern long do_fpu_taskswitch(int set);

extern long do_set_debugreg(int reg, unsigned long value);

extern unsigned long do_get_debugreg(int reg);

extern long do_update_descriptor(u64 pa, u64 desc);

extern int do_update_va_mapping(u32 va, u32 flags, u64 val64);

extern long do_physdev_op(GUEST_HANDLE(physdev_op_t) uop);

extern int do_update_va_mapping_otherdomain(unsigned long va,
					    u64 val64,
					    unsigned long flags,
					    domid_t domid);

extern int do_mmuext_op(GUEST_HANDLE(mmuext_op_t) uops,
			unsigned int count,
			GUEST_HANDLE(uint) pdone,
			unsigned int foreigndom);

extern unsigned long do_iret(void);

struct vcpu;
extern long arch_do_vcpu_op(int cmd, struct vcpu *v, GUEST_HANDLE(void) arg);

extern long do_set_callbacks(unsigned long event, unsigned long failsafe);
#endif
