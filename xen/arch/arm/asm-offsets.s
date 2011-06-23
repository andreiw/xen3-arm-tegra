	.file	"asm-offsets.c"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.align	2
	.global	main
	.type	main, %function
main:
.LFB111:
	.file 1 "xen/asm-offsets.c"
	.loc 1 59 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ lr needed for prologue
	.loc 1 60 0
#APP
	
->OFFSET_SOFTIRQ_PENDING #0 offsetof(struct irq_cpu_stat, __softirq_pending)
	.loc 1 61 0
	
->OFFSET_LOCAL_IRQ_COUNT #4 offsetof(struct irq_cpu_stat, __local_irq_count)
	.loc 1 62 0
	
->OFFSET_NMI_COUNT #8 offsetof(struct irq_cpu_stat, __nmi_count)
	.loc 1 63 0
	
->SIZE_IRQ_CPU_STAT #32 sizeof(struct irq_cpu_stat)
	.loc 1 64 0
	
->
	.loc 1 65 0
	
->OFFSET_VCPU #0 offsetof(struct cpu_info, vcpu)
	.loc 1 66 0
	
->OFFSET_VPSR #4 offsetof(struct cpu_info, vspsr)
	.loc 1 67 0
	
->OFFSET_VSP #8 offsetof(struct cpu_info, vsp)
	.loc 1 68 0
	
->OFFSET_VLR #12 offsetof(struct cpu_info, vlr)
	.loc 1 72 0
	
->
	.loc 1 73 0
	
->OFFSET_VCPU_INFO #8 offsetof(struct vcpu, vcpu_info)
	.loc 1 74 0
	
->OFFSET_ARCH_VCPU #224 offsetof(struct vcpu, arch)
	.loc 1 75 0
	
->
	.loc 1 76 0
	
->OFFSET_EVTCHN_UPCALL_MASK #1 offsetof(struct vcpu_info, evtchn_upcall_mask)
	.loc 1 77 0
	
->OFFSET_EVTCHN_UPCALL_PENDING #0 offsetof(struct vcpu_info, evtchn_upcall_pending)
	.loc 1 78 0
	
->OFFSET_ARCH_VCPU_INFO #8 offsetof(struct vcpu_info, arch)
	.loc 1 79 0
	
->OFFSET_TSP #0 offsetof(struct arch_vcpu_info, sp)
	.loc 1 80 0
	
->OFFSET_TLR #4 offsetof(struct arch_vcpu_info, lr)
	.loc 1 81 0
	
->OFFSET_TSPSR #12 offsetof(struct arch_vcpu_info, spsr)
	.loc 1 82 0
	
->OFFSET_TRAP #8 offsetof(struct arch_vcpu_info, trap)
	.loc 1 83 0
	
->OFFSET_VCR #16 offsetof(struct arch_vcpu_info, cr)
	.loc 1 84 0
	
->OFFSET_VDACR #24 offsetof(struct arch_vcpu_info, dacr)
	.loc 1 85 0
	
->OFFSET_VCPAR #20 offsetof(struct arch_vcpu_info, cpar)
	.loc 1 86 0
	
->OFFSET_VPIDR #28 offsetof(struct arch_vcpu_info, pidr)
	.loc 1 87 0
	
->OFFSET_VFSR #36 offsetof(struct arch_vcpu_info, fsr)
	.loc 1 88 0
	
->OFFSET_VFAR #32 offsetof(struct arch_vcpu_info, far)
	.loc 1 89 0
	
->
	.loc 1 90 0
	
->OFFSET_GUEST_CONTEXT #56 offsetof(struct arch_vcpu, guest_context)
	.loc 1 91 0
	
->OFFSET_TRAP_TABLE #20 offsetof(struct arch_vcpu, trap_table)
	.loc 1 92 0
	
->OFFSET_VECTOR_RESET #0 0
	.loc 1 93 0
	
->OFFSET_VECTOR_UND #4 4
	.loc 1 94 0
	
->OFFSET_VECTOR_SWI #8 8
	.loc 1 95 0
	
->OFFSET_VECTOR_PABT #12 12
	.loc 1 96 0
	
->OFFSET_VECTOR_DABT #16 16
	.loc 1 97 0
	
->OFFSET_VECTOR_IRQ #24 24
	.loc 1 98 0
	
->OFFSET_VECTOR_FIQ #28 28
	.loc 1 99 0
	
->
	.loc 1 100 0
	
->OFFSET_USER_REGS #0 offsetof(struct vcpu_guest_context, user_regs)
	.loc 1 101 0
	
->OFFSET_EXT_REGS #72 offsetof(struct vcpu_guest_context, ext_regs)
	.loc 1 102 0
	
->OFFSET_SYS_REGS #232 offsetof(struct vcpu_guest_context, sys_regs)
	.loc 1 103 0
	
->OFFSET_HYPERVISOR_CALLBACK #248 offsetof(struct vcpu_guest_context, event_callback)
	.loc 1 104 0
	
->OFFSET_FAILSAFE_CALLBACK #252 offsetof(struct vcpu_guest_context, failsafe_callback)
	.loc 1 105 0
	
->
	.loc 1 106 0
	
->OFFSET_R0 #0 offsetof(struct cpu_user_regs, r0)
	.loc 1 107 0
	
->OFFSET_R1 #4 offsetof(struct cpu_user_regs, r1)
	.loc 1 108 0
	
->OFFSET_R2 #8 offsetof(struct cpu_user_regs, r2)
	.loc 1 109 0
	
->OFFSET_R3 #12 offsetof(struct cpu_user_regs, r3)
	.loc 1 110 0
	
->OFFSET_R4 #16 offsetof(struct cpu_user_regs, r4)
	.loc 1 111 0
	
->OFFSET_R5 #20 offsetof(struct cpu_user_regs, r5)
	.loc 1 112 0
	
->OFFSET_R6 #24 offsetof(struct cpu_user_regs, r6)
	.loc 1 113 0
	
->OFFSET_R7 #28 offsetof(struct cpu_user_regs, r7)
	.loc 1 114 0
	
->OFFSET_R8 #32 offsetof(struct cpu_user_regs, r8)
	.loc 1 115 0
	
->OFFSET_R9 #36 offsetof(struct cpu_user_regs, r9)
	.loc 1 116 0
	
->OFFSET_R10 #40 offsetof(struct cpu_user_regs, r10)
	.loc 1 117 0
	
->OFFSET_R11 #44 offsetof(struct cpu_user_regs, r11)
	.loc 1 118 0
	
->OFFSET_R12 #48 offsetof(struct cpu_user_regs, r12)
	.loc 1 119 0
	
->OFFSET_R13 #52 offsetof(struct cpu_user_regs, r13)
	.loc 1 120 0
	
->OFFSET_R14 #56 offsetof(struct cpu_user_regs, r14)
	.loc 1 121 0
	
->OFFSET_R15 #60 offsetof(struct cpu_user_regs, r15)
	.loc 1 122 0
	
->OFFSET_PSR #64 offsetof(struct cpu_user_regs, psr)
	.loc 1 123 0
	
->OFFSET_CTX #68 offsetof(struct cpu_user_regs, ctx)
	.loc 1 124 0
	
->
	.loc 1 125 0
	
->OFFSET_PIDR #12 offsetof(struct cpu_sys_regs, pidr)
	.loc 1 126 0
	
->OFFSET_DACR #8 offsetof(struct cpu_sys_regs, dacr)
	.loc 1 127 0
	
->OFFSET_CR #4 offsetof(struct cpu_sys_regs, cr)
	.loc 1 128 0
	
->OFFSET_CPAR #0 offsetof(struct cpu_sys_regs, cpar)
	.loc 1 129 0
	
->
	.loc 1 130 0
	
->OFFSET_SOFTIRQ_PENDING #0 offsetof(struct irq_cpu_stat, __softirq_pending)
	.loc 1 131 0
	
->OFFSET_LOCAL_IRQ_COUNT #4 offsetof(struct irq_cpu_stat, __local_irq_count)
	.loc 1 134 0
	mov	r0, #0
	bx	lr
.LFE111:
	.size	main, .-main
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.byte	0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0x0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB111
	.4byte	.LFE111-.LFB111
	.align	2
.LEFDE0:
	.file 2 "/home/nirvana/release/opensource/xen-arm/xen/include/xen/irq_cpustat.h"
	.file 3 "/home/nirvana/release/opensource/xen-arm/xen/include/asm/hardirq.h"
	.file 4 "/home/nirvana/release/opensource/xen-arm/xen/include/asm/time.h"
	.file 5 "/home/nirvana/release/opensource/xen-arm/xen/include/asm/types.h"
	.text
.Letext0:
	.section	.debug_info
	.4byte	0x104
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.Ldebug_line0
	.4byte	.Letext0
	.4byte	.Ltext0
	.4byte	.LASF16
	.byte	0x1
	.4byte	.LASF17
	.4byte	.LASF18
	.uleb128 0x2
	.4byte	.LASF0
	.byte	0x1
	.byte	0x6
	.uleb128 0x2
	.4byte	.LASF1
	.byte	0x1
	.byte	0x8
	.uleb128 0x2
	.4byte	.LASF2
	.byte	0x2
	.byte	0x5
	.uleb128 0x2
	.4byte	.LASF3
	.byte	0x2
	.byte	0x7
	.uleb128 0x3
	.ascii	"int\000"
	.byte	0x4
	.byte	0x5
	.uleb128 0x2
	.4byte	.LASF4
	.byte	0x4
	.byte	0x7
	.uleb128 0x2
	.4byte	.LASF5
	.byte	0x8
	.byte	0x5
	.uleb128 0x2
	.4byte	.LASF6
	.byte	0x8
	.byte	0x7
	.uleb128 0x2
	.4byte	.LASF7
	.byte	0x4
	.byte	0x7
	.uleb128 0x4
	.ascii	"u64\000"
	.byte	0x5
	.byte	0x2a
	.4byte	0x56
	.uleb128 0x2
	.4byte	.LASF4
	.byte	0x4
	.byte	0x7
	.uleb128 0x2
	.4byte	.LASF8
	.byte	0x1
	.byte	0x8
	.uleb128 0x2
	.4byte	.LASF9
	.byte	0x4
	.byte	0x5
	.uleb128 0x5
	.4byte	0xbb
	.4byte	.LASF19
	.byte	0x20
	.byte	0x3
	.byte	0x7
	.uleb128 0x6
	.4byte	.LASF10
	.byte	0x3
	.byte	0x8
	.4byte	0x5d
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x6
	.4byte	.LASF11
	.byte	0x3
	.byte	0x9
	.4byte	0x5d
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x6
	.4byte	.LASF12
	.byte	0x3
	.byte	0xa
	.4byte	0x5d
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF13
	.byte	0x3
	.byte	0xb
	.4byte	0x84
	.uleb128 0x8
	.byte	0x1
	.4byte	.LASF20
	.byte	0x1
	.byte	0x3b
	.byte	0x1
	.4byte	0x41
	.4byte	.LFB111
	.4byte	.LFE111
	.byte	0x1
	.byte	0x5d
	.uleb128 0x9
	.4byte	0xe8
	.4byte	0xbb
	.uleb128 0xa
	.byte	0x0
	.uleb128 0xb
	.4byte	.LASF14
	.byte	0x2
	.byte	0x15
	.4byte	0xf5
	.byte	0x1
	.byte	0x1
	.uleb128 0xc
	.4byte	0xdd
	.uleb128 0xb
	.4byte	.LASF15
	.byte	0x4
	.byte	0x1e
	.4byte	0x64
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x10
	.uleb128 0x6
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x21
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x17
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x108
	.4byte	0xc6
	.ascii	"main\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF2:
	.ascii	"short int\000"
.LASF19:
	.ascii	"irq_cpu_stat\000"
.LASF6:
	.ascii	"long long unsigned int\000"
.LASF15:
	.ascii	"jiffies_64\000"
.LASF12:
	.ascii	"__nmi_count\000"
.LASF8:
	.ascii	"char\000"
.LASF5:
	.ascii	"long long int\000"
.LASF10:
	.ascii	"__softirq_pending\000"
.LASF9:
	.ascii	"long int\000"
.LASF11:
	.ascii	"__local_irq_count\000"
.LASF4:
	.ascii	"unsigned int\000"
.LASF7:
	.ascii	"long unsigned int\000"
.LASF16:
	.ascii	"GNU C 4.1.1\000"
.LASF1:
	.ascii	"unsigned char\000"
.LASF0:
	.ascii	"signed char\000"
.LASF20:
	.ascii	"main\000"
.LASF3:
	.ascii	"short unsigned int\000"
.LASF14:
	.ascii	"irq_stat\000"
.LASF18:
	.ascii	"/home/nirvana/release/opensource/xen-arm/xen/arch/a"
	.ascii	"rm\000"
.LASF13:
	.ascii	"irq_cpustat_t\000"
.LASF17:
	.ascii	"xen/asm-offsets.c\000"
	.ident	"GCC: (GNU) 4.1.1"
