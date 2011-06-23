/**
 * @file	arch-arm.h	(include/public/arch-arm.h)
 * @brief	archtecture specifiec configuration
 * 
 * @author	chanju, park	(beastworld@samsung.com)
 * @version	2006-06-08	basic setup, modified from ia-64, ppc
 * @copyright	Copyright (c) samsung electronics, co.
 */

#ifndef __XEN_PUBLIC_ARCH_ARM_32_H__
#define __XEN_PUBLIC_ARCH_ARM_32_H__

#define CMD_FMRX		0
#define CMD_FMXR		1

#define FPEXC_XEN       0
#define FPINST_XEN      1
#define FPINST2_XEN     2

/* FPEXC bits */
#define FPEXC_EXCEPTION     (1<<31)
#define FPEXC_ENABLE        (1<<30)

#ifndef __ASSEMBLY__
#ifdef __XEN__
#define __DEFINE_GUEST_HANDLE(name, type) \
    typedef struct { type *p; } __guest_handle_ ## name
#else
#define __DEFINE_GUEST_HANDLE(name, type) \
    typedef type * __guest_handle_ ## name
#endif
    
#define DEFINE_GUEST_HANDLE(name) __DEFINE_GUEST_HANDLE(name, name)
#define GUEST_HANDLE(name)        __guest_handle_ ## name
    
/* Guest handles for primitive C types. */
__DEFINE_GUEST_HANDLE(uchar, unsigned char);
__DEFINE_GUEST_HANDLE(uint,  unsigned int);
__DEFINE_GUEST_HANDLE(ulong, unsigned long);
DEFINE_GUEST_HANDLE(char);
DEFINE_GUEST_HANDLE(int);
DEFINE_GUEST_HANDLE(long);
DEFINE_GUEST_HANDLE(void);


/*
 * Virtual addresses beyond this are not modifiable by guest OSes. The 
 * machine->physical mapping table starts at this address, read-only.
 */
#define __HYPERVISOR_VIRT_START 0xFC000000

#ifndef HYPERVISOR_VIRT_START
#define HYPERVISOR_VIRT_START mk_unsigned_long(__HYPERVISOR_VIRT_START)
#endif

#ifndef machine_to_phys_mapping
#define machine_to_phys_mapping ((unsigned long *)HYPERVISOR_VIRT_START)
#endif

typedef struct cpu_user_regs
{
	unsigned long	r0;
	unsigned long	r1;
	unsigned long	r2;
	unsigned long	r3;
	unsigned long	r4;
	unsigned long	r5;
	unsigned long	r6;
	unsigned long	r7;
	unsigned long	r8;
	unsigned long	r9;
	unsigned long	r10;
	unsigned long	r11;
	unsigned long	r12;
	unsigned long	r13;
	unsigned long	r14;
	unsigned long	r15;
	unsigned long	psr;
	unsigned long	ctx;
}cpu_user_regs_t;

typedef struct cpu_ext_regs {
	unsigned long long 	wr0;
	unsigned long long 	wr1;
	unsigned long long 	wr2;
	unsigned long long 	wr3;
	unsigned long long 	wr4;
	unsigned long long 	wr5;
	unsigned long long 	wr6;
	unsigned long long 	wr7;
	unsigned long long 	wr8;
	unsigned long long 	wr9;
	unsigned long long 	wr10;
	unsigned long long 	wr11;
	unsigned long long 	wr12;
	unsigned long long 	wr13;
	unsigned long long 	wr14;
	unsigned long long 	wr15;
	unsigned long	 	wcssf;
	unsigned long 		wcasf;
	unsigned long 		wcgr0;
	unsigned long 		wcgr1;
	unsigned long 		wcgr2;
	unsigned long 		wcgr3;
	unsigned long 		wcid;
	unsigned long 		wcon;
}cpu_ext_regs_t;

typedef struct cpu_sys_regs {
	unsigned long 	cpar;
	unsigned long 	cr;
	unsigned long 	dacr;
	unsigned long 	pidr;
}cpu_sys_regs_t;

typedef cpu_user_regs_t	cpu_bounce_frame_t;

#define DEFINE_GUEST_HANDLE(name) __DEFINE_GUEST_HANDLE(name, name)
#define GUEST_HANDLE(name)        __guest_handle_ ## name

typedef struct trap_info {
	unsigned long flags;
	unsigned long address;
}trap_info_t;
DEFINE_GUEST_HANDLE(trap_info_t);

typedef struct vcpu_guest_context {
	cpu_user_regs_t user_regs;			/* User-level CPU registers     */
	cpu_ext_regs_t	ext_regs;
	cpu_sys_regs_t	sys_regs;
	unsigned long	event_callback;
	unsigned long	failsafe_callback;	/* Address of failsafe callback  */
} vcpu_guest_context_t;
DEFINE_GUEST_HANDLE(vcpu_guest_context_t);

typedef struct trap_frame {
	unsigned long sp;
	unsigned long lr;
	unsigned long spsr;
	unsigned long trap;
}trap_frame_t;
DEFINE_GUEST_HANDLE(trap_frame_t);

typedef struct arch_vcpu_info {
	unsigned long	sp;
	unsigned long	lr;
	unsigned long	trap;
	unsigned long	spsr;
	unsigned long	cr;
	unsigned long	cpar;
	unsigned long	dacr;
	unsigned long	pidr;
	unsigned long	far;
	unsigned long	fsr;
	unsigned long	reserved10;
	unsigned long	reserved11;
	unsigned long	reserved12;
	unsigned long	reserved13;
	unsigned long	reserved14;
} arch_vcpu_info_t;

#define MAX_VIRT_CPUS 1

typedef struct arch_shared_info {
	unsigned long	max_pfn;
	unsigned long	pfn_to_mfn_frame_list_list;
	unsigned long	nmi_reason;
} arch_shared_info_t;

#endif
#endif
