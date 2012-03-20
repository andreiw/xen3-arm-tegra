	.arch armv7-a
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 2
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
.LFB109:
	.file 1 "xen/asm-offsets.c"
	.loc 1 59 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
.LCFI0:
	stmfd	sp!, {fp, ip, lr, pc}
.LCFI1:
	sub	fp, ip, #4
.LCFI2:
	.loc 1 60 0
@ 60 "xen/asm-offsets.c" 1
	
->OFFSET_SOFTIRQ_PENDING #0 offsetof(struct irq_cpu_stat, __softirq_pending)
@ 0 "" 2
	.loc 1 61 0
@ 61 "xen/asm-offsets.c" 1
	
->OFFSET_LOCAL_IRQ_COUNT #4 offsetof(struct irq_cpu_stat, __local_irq_count)
@ 0 "" 2
	.loc 1 62 0
@ 62 "xen/asm-offsets.c" 1
	
->OFFSET_NMI_COUNT #8 offsetof(struct irq_cpu_stat, __nmi_count)
@ 0 "" 2
	.loc 1 63 0
@ 63 "xen/asm-offsets.c" 1
	
->SIZE_IRQ_CPU_STAT #32 sizeof(struct irq_cpu_stat)
@ 0 "" 2
	.loc 1 64 0
@ 64 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 65 0
@ 65 "xen/asm-offsets.c" 1
	
->OFFSET_VCPU #0 offsetof(struct cpu_info, vcpu)
@ 0 "" 2
	.loc 1 66 0
@ 66 "xen/asm-offsets.c" 1
	
->OFFSET_VPSR #4 offsetof(struct cpu_info, vspsr)
@ 0 "" 2
	.loc 1 67 0
@ 67 "xen/asm-offsets.c" 1
	
->OFFSET_VSP #8 offsetof(struct cpu_info, vsp)
@ 0 "" 2
	.loc 1 68 0
@ 68 "xen/asm-offsets.c" 1
	
->OFFSET_VLR #12 offsetof(struct cpu_info, vlr)
@ 0 "" 2
	.loc 1 72 0
@ 72 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 73 0
@ 73 "xen/asm-offsets.c" 1
	
->OFFSET_VCPU_INFO #8 offsetof(struct vcpu, vcpu_info)
@ 0 "" 2
	.loc 1 74 0
@ 74 "xen/asm-offsets.c" 1
	
->OFFSET_ARCH_VCPU #224 offsetof(struct vcpu, arch)
@ 0 "" 2
	.loc 1 75 0
@ 75 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 76 0
@ 76 "xen/asm-offsets.c" 1
	
->OFFSET_EVTCHN_UPCALL_MASK #1 offsetof(struct vcpu_info, evtchn_upcall_mask)
@ 0 "" 2
	.loc 1 77 0
@ 77 "xen/asm-offsets.c" 1
	
->OFFSET_EVTCHN_UPCALL_PENDING #0 offsetof(struct vcpu_info, evtchn_upcall_pending)
@ 0 "" 2
	.loc 1 78 0
@ 78 "xen/asm-offsets.c" 1
	
->OFFSET_ARCH_VCPU_INFO #8 offsetof(struct vcpu_info, arch)
@ 0 "" 2
	.loc 1 79 0
@ 79 "xen/asm-offsets.c" 1
	
->OFFSET_TSP #0 offsetof(struct arch_vcpu_info, sp)
@ 0 "" 2
	.loc 1 80 0
@ 80 "xen/asm-offsets.c" 1
	
->OFFSET_TLR #4 offsetof(struct arch_vcpu_info, lr)
@ 0 "" 2
	.loc 1 81 0
@ 81 "xen/asm-offsets.c" 1
	
->OFFSET_TSPSR #12 offsetof(struct arch_vcpu_info, spsr)
@ 0 "" 2
	.loc 1 82 0
@ 82 "xen/asm-offsets.c" 1
	
->OFFSET_TRAP #8 offsetof(struct arch_vcpu_info, trap)
@ 0 "" 2
	.loc 1 83 0
@ 83 "xen/asm-offsets.c" 1
	
->OFFSET_VCR #16 offsetof(struct arch_vcpu_info, cr)
@ 0 "" 2
	.loc 1 84 0
@ 84 "xen/asm-offsets.c" 1
	
->OFFSET_VDACR #24 offsetof(struct arch_vcpu_info, dacr)
@ 0 "" 2
	.loc 1 85 0
@ 85 "xen/asm-offsets.c" 1
	
->OFFSET_VCPAR #20 offsetof(struct arch_vcpu_info, cpar)
@ 0 "" 2
	.loc 1 86 0
@ 86 "xen/asm-offsets.c" 1
	
->OFFSET_VPIDR #28 offsetof(struct arch_vcpu_info, pidr)
@ 0 "" 2
	.loc 1 87 0
@ 87 "xen/asm-offsets.c" 1
	
->OFFSET_VFSR #36 offsetof(struct arch_vcpu_info, fsr)
@ 0 "" 2
	.loc 1 88 0
@ 88 "xen/asm-offsets.c" 1
	
->OFFSET_VFAR #32 offsetof(struct arch_vcpu_info, far)
@ 0 "" 2
	.loc 1 89 0
@ 89 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 90 0
@ 90 "xen/asm-offsets.c" 1
	
->OFFSET_GUEST_CONTEXT #56 offsetof(struct arch_vcpu, guest_context)
@ 0 "" 2
	.loc 1 91 0
@ 91 "xen/asm-offsets.c" 1
	
->OFFSET_TRAP_TABLE #20 offsetof(struct arch_vcpu, trap_table)
@ 0 "" 2
	.loc 1 92 0
@ 92 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_RESET #0 0
@ 0 "" 2
	.loc 1 93 0
@ 93 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_UND #4 4
@ 0 "" 2
	.loc 1 94 0
@ 94 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_SWI #8 8
@ 0 "" 2
	.loc 1 95 0
@ 95 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_PABT #12 12
@ 0 "" 2
	.loc 1 96 0
@ 96 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_DABT #16 16
@ 0 "" 2
	.loc 1 97 0
@ 97 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_IRQ #24 24
@ 0 "" 2
	.loc 1 98 0
@ 98 "xen/asm-offsets.c" 1
	
->OFFSET_VECTOR_FIQ #28 28
@ 0 "" 2
	.loc 1 99 0
@ 99 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 100 0
@ 100 "xen/asm-offsets.c" 1
	
->OFFSET_USER_REGS #0 offsetof(struct vcpu_guest_context, user_regs)
@ 0 "" 2
	.loc 1 101 0
@ 101 "xen/asm-offsets.c" 1
	
->OFFSET_EXT_REGS #72 offsetof(struct vcpu_guest_context, ext_regs)
@ 0 "" 2
	.loc 1 102 0
@ 102 "xen/asm-offsets.c" 1
	
->OFFSET_SYS_REGS #232 offsetof(struct vcpu_guest_context, sys_regs)
@ 0 "" 2
	.loc 1 103 0
@ 103 "xen/asm-offsets.c" 1
	
->OFFSET_HYPERVISOR_CALLBACK #248 offsetof(struct vcpu_guest_context, event_callback)
@ 0 "" 2
	.loc 1 104 0
@ 104 "xen/asm-offsets.c" 1
	
->OFFSET_FAILSAFE_CALLBACK #252 offsetof(struct vcpu_guest_context, failsafe_callback)
@ 0 "" 2
	.loc 1 105 0
@ 105 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 106 0
@ 106 "xen/asm-offsets.c" 1
	
->OFFSET_R0 #0 offsetof(struct cpu_user_regs, r0)
@ 0 "" 2
	.loc 1 107 0
@ 107 "xen/asm-offsets.c" 1
	
->OFFSET_R1 #4 offsetof(struct cpu_user_regs, r1)
@ 0 "" 2
	.loc 1 108 0
@ 108 "xen/asm-offsets.c" 1
	
->OFFSET_R2 #8 offsetof(struct cpu_user_regs, r2)
@ 0 "" 2
	.loc 1 109 0
@ 109 "xen/asm-offsets.c" 1
	
->OFFSET_R3 #12 offsetof(struct cpu_user_regs, r3)
@ 0 "" 2
	.loc 1 110 0
@ 110 "xen/asm-offsets.c" 1
	
->OFFSET_R4 #16 offsetof(struct cpu_user_regs, r4)
@ 0 "" 2
	.loc 1 111 0
@ 111 "xen/asm-offsets.c" 1
	
->OFFSET_R5 #20 offsetof(struct cpu_user_regs, r5)
@ 0 "" 2
	.loc 1 112 0
@ 112 "xen/asm-offsets.c" 1
	
->OFFSET_R6 #24 offsetof(struct cpu_user_regs, r6)
@ 0 "" 2
	.loc 1 113 0
@ 113 "xen/asm-offsets.c" 1
	
->OFFSET_R7 #28 offsetof(struct cpu_user_regs, r7)
@ 0 "" 2
	.loc 1 114 0
@ 114 "xen/asm-offsets.c" 1
	
->OFFSET_R8 #32 offsetof(struct cpu_user_regs, r8)
@ 0 "" 2
	.loc 1 115 0
@ 115 "xen/asm-offsets.c" 1
	
->OFFSET_R9 #36 offsetof(struct cpu_user_regs, r9)
@ 0 "" 2
	.loc 1 116 0
@ 116 "xen/asm-offsets.c" 1
	
->OFFSET_R10 #40 offsetof(struct cpu_user_regs, r10)
@ 0 "" 2
	.loc 1 117 0
@ 117 "xen/asm-offsets.c" 1
	
->OFFSET_R11 #44 offsetof(struct cpu_user_regs, r11)
@ 0 "" 2
	.loc 1 118 0
@ 118 "xen/asm-offsets.c" 1
	
->OFFSET_R12 #48 offsetof(struct cpu_user_regs, r12)
@ 0 "" 2
	.loc 1 119 0
@ 119 "xen/asm-offsets.c" 1
	
->OFFSET_R13 #52 offsetof(struct cpu_user_regs, r13)
@ 0 "" 2
	.loc 1 120 0
@ 120 "xen/asm-offsets.c" 1
	
->OFFSET_R14 #56 offsetof(struct cpu_user_regs, r14)
@ 0 "" 2
	.loc 1 121 0
@ 121 "xen/asm-offsets.c" 1
	
->OFFSET_R15 #60 offsetof(struct cpu_user_regs, r15)
@ 0 "" 2
	.loc 1 122 0
@ 122 "xen/asm-offsets.c" 1
	
->OFFSET_PSR #64 offsetof(struct cpu_user_regs, psr)
@ 0 "" 2
	.loc 1 123 0
@ 123 "xen/asm-offsets.c" 1
	
->OFFSET_CTX #68 offsetof(struct cpu_user_regs, ctx)
@ 0 "" 2
	.loc 1 124 0
@ 124 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 125 0
@ 125 "xen/asm-offsets.c" 1
	
->OFFSET_PIDR #12 offsetof(struct cpu_sys_regs, pidr)
@ 0 "" 2
	.loc 1 126 0
@ 126 "xen/asm-offsets.c" 1
	
->OFFSET_DACR #8 offsetof(struct cpu_sys_regs, dacr)
@ 0 "" 2
	.loc 1 127 0
@ 127 "xen/asm-offsets.c" 1
	
->OFFSET_CR #4 offsetof(struct cpu_sys_regs, cr)
@ 0 "" 2
	.loc 1 128 0
@ 128 "xen/asm-offsets.c" 1
	
->OFFSET_CPAR #0 offsetof(struct cpu_sys_regs, cpar)
@ 0 "" 2
	.loc 1 129 0
@ 129 "xen/asm-offsets.c" 1
	
->
@ 0 "" 2
	.loc 1 130 0
@ 130 "xen/asm-offsets.c" 1
	
->OFFSET_SOFTIRQ_PENDING #0 offsetof(struct irq_cpu_stat, __softirq_pending)
@ 0 "" 2
	.loc 1 131 0
@ 131 "xen/asm-offsets.c" 1
	
->OFFSET_LOCAL_IRQ_COUNT #4 offsetof(struct irq_cpu_stat, __local_irq_count)
@ 0 "" 2
	.loc 1 133 0
	mov	r3, #0
	.loc 1 134 0
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.LFE109:
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
	.4byte	.LFB109
	.4byte	.LFE109-.LFB109
	.byte	0x4
	.4byte	.LCFI0-.LFB109
	.byte	0xd
	.uleb128 0xc
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0x11
	.uleb128 0xe
	.sleb128 2
	.byte	0x11
	.uleb128 0xd
	.sleb128 3
	.byte	0x11
	.uleb128 0xb
	.sleb128 4
	.byte	0x4
	.4byte	.LCFI2-.LCFI1
	.byte	0xc
	.uleb128 0xb
	.uleb128 0x4
	.align	2
.LEFDE0:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB109-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0-.Ltext0
	.4byte	.LCFI2-.Ltext0
	.2byte	0x2
	.byte	0x7c
	.sleb128 0
	.4byte	.LCFI2-.Ltext0
	.4byte	.LFE109-.Ltext0
	.2byte	0x2
	.byte	0x7b
	.sleb128 4
	.4byte	0x0
	.4byte	0x0
	.file 2 "/home/andreiw/src/xen3-arm-tegra/xen/include/asm/types.h"
	.file 3 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/types.h"
	.file 4 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/list.h"
	.file 5 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/spinlock.h"
	.file 6 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/cpumask.h"
	.file 7 "/home/andreiw/src/xen3-arm-tegra/xen/include/public/arch-arm.h"
	.file 8 "/home/andreiw/src/xen3-arm-tegra/xen/include/public/xen.h"
	.file 9 "/home/andreiw/src/xen3-arm-tegra/xen/include/public/vcpu.h"
	.file 10 "/home/andreiw/src/xen3-arm-tegra/xen/include/asm/current.h"
	.file 11 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/sched.h"
	.file 12 "/home/andreiw/src/xen3-arm-tegra/xen/include/asm/hardirq.h"
	.file 13 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/time.h"
	.file 14 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/timer.h"
	.file 15 "/home/andreiw/src/xen3-arm-tegra/xen/include/public/grant_table.h"
	.file 16 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/grant_table.h"
	.file 17 "/home/andreiw/src/xen3-arm-tegra/xen/include/asm/domain.h"
	.file 18 "/home/andreiw/src/xen3-arm-tegra/xen/include/asm/atomic.h"
	.file 19 "/home/andreiw/src/xen3-arm-tegra/xen/include/xen/irq_cpustat.h"
	.file 20 "/home/andreiw/src/xen3-arm-tegra/xen/include/asm/time.h"
	.section	.debug_info
	.4byte	0x100e
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF202
	.byte	0x1
	.4byte	.LASF203
	.4byte	.LASF204
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.4byte	.LASF0
	.byte	0x2
	.byte	0xe
	.4byte	0x30
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.4byte	.LASF2
	.uleb128 0x2
	.4byte	.LASF1
	.byte	0x2
	.byte	0xf
	.4byte	0x42
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF3
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x2
	.4byte	.LASF5
	.byte	0x2
	.byte	0x12
	.4byte	0x5b
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.4byte	.LASF7
	.byte	0x2
	.byte	0x15
	.4byte	0x74
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.4byte	.LASF9
	.uleb128 0x2
	.4byte	.LASF10
	.byte	0x2
	.byte	0x19
	.4byte	0x8d
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.4byte	.LASF11
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF12
	.uleb128 0x5
	.ascii	"u16\000"
	.byte	0x2
	.byte	0x24
	.4byte	0x5b
	.uleb128 0x5
	.ascii	"u32\000"
	.byte	0x2
	.byte	0x27
	.4byte	0x74
	.uleb128 0x5
	.ascii	"s64\000"
	.byte	0x2
	.byte	0x29
	.4byte	0x7b
	.uleb128 0x5
	.ascii	"u64\000"
	.byte	0x2
	.byte	0x2a
	.4byte	0x8d
	.uleb128 0x2
	.4byte	.LASF13
	.byte	0x3
	.byte	0x23
	.4byte	0x37
	.uleb128 0x2
	.4byte	.LASF14
	.byte	0x3
	.byte	0x25
	.4byte	0x25
	.uleb128 0x2
	.4byte	.LASF15
	.byte	0x3
	.byte	0x27
	.4byte	0x50
	.uleb128 0x2
	.4byte	.LASF16
	.byte	0x3
	.byte	0x2b
	.4byte	0x69
	.uleb128 0x2
	.4byte	.LASF17
	.byte	0x3
	.byte	0x2f
	.4byte	0x82
	.uleb128 0x6
	.byte	0x4
	.uleb128 0x7
	.4byte	.LASF25
	.byte	0x8
	.byte	0x4
	.byte	0x10
	.4byte	0x129
	.uleb128 0x8
	.4byte	.LASF18
	.byte	0x4
	.byte	0x11
	.4byte	0x129
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF19
	.byte	0x4
	.byte	0x11
	.4byte	0x129
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0x100
	.uleb128 0xa
	.byte	0x0
	.byte	0x5
	.byte	0x2c
	.uleb128 0x2
	.4byte	.LASF20
	.byte	0x5
	.byte	0x2c
	.4byte	0x12f
	.uleb128 0xb
	.byte	0x4
	.byte	0x6
	.byte	0x50
	.4byte	0x155
	.uleb128 0x8
	.4byte	.LASF21
	.byte	0x6
	.byte	0x50
	.4byte	0x155
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0xc
	.4byte	0x94
	.4byte	0x165
	.uleb128 0xd
	.4byte	0x165
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.byte	0x4
	.byte	0x7
	.uleb128 0x2
	.4byte	.LASF22
	.byte	0x6
	.byte	0x50
	.4byte	0x13e
	.uleb128 0x9
	.byte	0x4
	.4byte	0x94
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.4byte	.LASF23
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.4byte	.LASF24
	.uleb128 0x7
	.4byte	.LASF26
	.byte	0x48
	.byte	0x7
	.byte	0x3d
	.4byte	0x286
	.uleb128 0xf
	.ascii	"r0\000"
	.byte	0x7
	.byte	0x3e
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xf
	.ascii	"r1\000"
	.byte	0x7
	.byte	0x3f
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xf
	.ascii	"r2\000"
	.byte	0x7
	.byte	0x40
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xf
	.ascii	"r3\000"
	.byte	0x7
	.byte	0x41
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xf
	.ascii	"r4\000"
	.byte	0x7
	.byte	0x42
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xf
	.ascii	"r5\000"
	.byte	0x7
	.byte	0x43
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xf
	.ascii	"r6\000"
	.byte	0x7
	.byte	0x44
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xf
	.ascii	"r7\000"
	.byte	0x7
	.byte	0x45
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xf
	.ascii	"r8\000"
	.byte	0x7
	.byte	0x46
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xf
	.ascii	"r9\000"
	.byte	0x7
	.byte	0x47
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0xf
	.ascii	"r10\000"
	.byte	0x7
	.byte	0x48
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xf
	.ascii	"r11\000"
	.byte	0x7
	.byte	0x49
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0xf
	.ascii	"r12\000"
	.byte	0x7
	.byte	0x4a
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0xf
	.ascii	"r13\000"
	.byte	0x7
	.byte	0x4b
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0xf
	.ascii	"r14\000"
	.byte	0x7
	.byte	0x4c
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0xf
	.ascii	"r15\000"
	.byte	0x7
	.byte	0x4d
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0xf
	.ascii	"psr\000"
	.byte	0x7
	.byte	0x4e
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0xf
	.ascii	"ctx\000"
	.byte	0x7
	.byte	0x4f
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF27
	.byte	0x7
	.byte	0x50
	.4byte	0x187
	.uleb128 0x7
	.4byte	.LASF28
	.byte	0xa0
	.byte	0x7
	.byte	0x52
	.4byte	0x3f6
	.uleb128 0xf
	.ascii	"wr0\000"
	.byte	0x7
	.byte	0x53
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xf
	.ascii	"wr1\000"
	.byte	0x7
	.byte	0x54
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xf
	.ascii	"wr2\000"
	.byte	0x7
	.byte	0x55
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xf
	.ascii	"wr3\000"
	.byte	0x7
	.byte	0x56
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xf
	.ascii	"wr4\000"
	.byte	0x7
	.byte	0x57
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xf
	.ascii	"wr5\000"
	.byte	0x7
	.byte	0x58
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xf
	.ascii	"wr6\000"
	.byte	0x7
	.byte	0x59
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0xf
	.ascii	"wr7\000"
	.byte	0x7
	.byte	0x5a
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0xf
	.ascii	"wr8\000"
	.byte	0x7
	.byte	0x5b
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0xf
	.ascii	"wr9\000"
	.byte	0x7
	.byte	0x5c
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0x8
	.4byte	.LASF29
	.byte	0x7
	.byte	0x5d
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0x8
	.4byte	.LASF30
	.byte	0x7
	.byte	0x5e
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x58
	.uleb128 0x8
	.4byte	.LASF31
	.byte	0x7
	.byte	0x5f
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0x8
	.4byte	.LASF32
	.byte	0x7
	.byte	0x60
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x68
	.uleb128 0x8
	.4byte	.LASF33
	.byte	0x7
	.byte	0x61
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x70
	.uleb128 0x8
	.4byte	.LASF34
	.byte	0x7
	.byte	0x62
	.4byte	0x8d
	.byte	0x2
	.byte	0x23
	.uleb128 0x78
	.uleb128 0x8
	.4byte	.LASF35
	.byte	0x7
	.byte	0x63
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x80
	.uleb128 0x8
	.4byte	.LASF36
	.byte	0x7
	.byte	0x64
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x84
	.uleb128 0x8
	.4byte	.LASF37
	.byte	0x7
	.byte	0x65
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x88
	.uleb128 0x8
	.4byte	.LASF38
	.byte	0x7
	.byte	0x66
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x8c
	.uleb128 0x8
	.4byte	.LASF39
	.byte	0x7
	.byte	0x67
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x90
	.uleb128 0x8
	.4byte	.LASF40
	.byte	0x7
	.byte	0x68
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x94
	.uleb128 0x8
	.4byte	.LASF41
	.byte	0x7
	.byte	0x69
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x98
	.uleb128 0x8
	.4byte	.LASF42
	.byte	0x7
	.byte	0x6a
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x9c
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF43
	.byte	0x7
	.byte	0x6b
	.4byte	0x291
	.uleb128 0x7
	.4byte	.LASF44
	.byte	0x10
	.byte	0x7
	.byte	0x6d
	.4byte	0x445
	.uleb128 0x8
	.4byte	.LASF45
	.byte	0x7
	.byte	0x6e
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xf
	.ascii	"cr\000"
	.byte	0x7
	.byte	0x6f
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF46
	.byte	0x7
	.byte	0x70
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF47
	.byte	0x7
	.byte	0x71
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF48
	.byte	0x7
	.byte	0x72
	.4byte	0x401
	.uleb128 0x10
	.4byte	.LASF49
	.2byte	0x100
	.byte	0x7
	.byte	0x7f
	.4byte	0x4a7
	.uleb128 0x8
	.4byte	.LASF50
	.byte	0x7
	.byte	0x80
	.4byte	0x286
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF51
	.byte	0x7
	.byte	0x81
	.4byte	0x3f6
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0x8
	.4byte	.LASF52
	.byte	0x7
	.byte	0x82
	.4byte	0x445
	.byte	0x3
	.byte	0x23
	.uleb128 0xe8
	.uleb128 0x8
	.4byte	.LASF53
	.byte	0x7
	.byte	0x83
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0xf8
	.uleb128 0x8
	.4byte	.LASF54
	.byte	0x7
	.byte	0x84
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0xfc
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF55
	.byte	0x3c
	.byte	0x7
	.byte	0x90
	.4byte	0x583
	.uleb128 0xf
	.ascii	"sp\000"
	.byte	0x7
	.byte	0x91
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xf
	.ascii	"lr\000"
	.byte	0x7
	.byte	0x92
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF56
	.byte	0x7
	.byte	0x93
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF57
	.byte	0x7
	.byte	0x94
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xf
	.ascii	"cr\000"
	.byte	0x7
	.byte	0x95
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF45
	.byte	0x7
	.byte	0x96
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x8
	.4byte	.LASF46
	.byte	0x7
	.byte	0x97
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x8
	.4byte	.LASF47
	.byte	0x7
	.byte	0x98
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xf
	.ascii	"far\000"
	.byte	0x7
	.byte	0x99
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xf
	.ascii	"fsr\000"
	.byte	0x7
	.byte	0x9a
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x8
	.4byte	.LASF58
	.byte	0x7
	.byte	0x9b
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x8
	.4byte	.LASF59
	.byte	0x7
	.byte	0x9c
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0x8
	.4byte	.LASF60
	.byte	0x7
	.byte	0x9d
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x8
	.4byte	.LASF61
	.byte	0x7
	.byte	0x9e
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0x8
	.4byte	.LASF62
	.byte	0x7
	.byte	0x9f
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF63
	.byte	0x7
	.byte	0xa0
	.4byte	0x4a7
	.uleb128 0x7
	.4byte	.LASF64
	.byte	0xc
	.byte	0x7
	.byte	0xa4
	.4byte	0x5c5
	.uleb128 0x8
	.4byte	.LASF65
	.byte	0x7
	.byte	0xa5
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF66
	.byte	0x7
	.byte	0xa6
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF67
	.byte	0x7
	.byte	0xa7
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF68
	.byte	0x7
	.byte	0xa8
	.4byte	0x58e
	.uleb128 0x11
	.4byte	.LASF69
	.byte	0x8
	.2byte	0x10a
	.4byte	0xdd
	.uleb128 0x12
	.4byte	.LASF70
	.byte	0x28
	.byte	0x8
	.2byte	0x140
	.4byte	0x644
	.uleb128 0x13
	.4byte	.LASF71
	.byte	0x8
	.2byte	0x14a
	.4byte	0xe8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x13
	.4byte	.LASF72
	.byte	0x8
	.2byte	0x14b
	.4byte	0xe8
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x13
	.4byte	.LASF73
	.byte	0x8
	.2byte	0x14c
	.4byte	0xe8
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x13
	.4byte	.LASF74
	.byte	0x8
	.2byte	0x14d
	.4byte	0xf3
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x13
	.4byte	.LASF75
	.byte	0x8
	.2byte	0x14e
	.4byte	0xf3
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x13
	.4byte	.LASF76
	.byte	0x8
	.2byte	0x14f
	.4byte	0x644
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.byte	0x0
	.uleb128 0xc
	.4byte	0xd2
	.4byte	0x654
	.uleb128 0xd
	.4byte	0x165
	.byte	0x2
	.byte	0x0
	.uleb128 0x11
	.4byte	.LASF77
	.byte	0x8
	.2byte	0x150
	.4byte	0x5dc
	.uleb128 0x12
	.4byte	.LASF78
	.byte	0x70
	.byte	0x8
	.2byte	0x152
	.4byte	0x6b9
	.uleb128 0x13
	.4byte	.LASF79
	.byte	0x8
	.2byte	0x16c
	.4byte	0xc7
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x13
	.4byte	.LASF80
	.byte	0x8
	.2byte	0x16d
	.4byte	0xc7
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x13
	.4byte	.LASF81
	.byte	0x8
	.2byte	0x16e
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x13
	.4byte	.LASF82
	.byte	0x8
	.2byte	0x16f
	.4byte	0x583
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x13
	.4byte	.LASF83
	.byte	0x8
	.2byte	0x170
	.4byte	0x654
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.byte	0x0
	.uleb128 0x11
	.4byte	.LASF84
	.byte	0x8
	.2byte	0x171
	.4byte	0x660
	.uleb128 0x14
	.4byte	.LASF85
	.2byte	0x1b8
	.byte	0x8
	.2byte	0x17a
	.4byte	0x792
	.uleb128 0x13
	.4byte	.LASF78
	.byte	0x8
	.2byte	0x17b
	.4byte	0x792
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x13
	.4byte	.LASF86
	.byte	0x8
	.2byte	0x19c
	.4byte	0x7a2
	.byte	0x2
	.byte	0x23
	.uleb128 0x70
	.uleb128 0x13
	.4byte	.LASF87
	.byte	0x8
	.2byte	0x19d
	.4byte	0x7a2
	.byte	0x3
	.byte	0x23
	.uleb128 0xf0
	.uleb128 0x13
	.4byte	.LASF88
	.byte	0x8
	.2byte	0x19e
	.4byte	0x7b2
	.byte	0x3
	.byte	0x23
	.uleb128 0x170
	.uleb128 0x13
	.4byte	.LASF89
	.byte	0x8
	.2byte	0x19f
	.4byte	0x7b2
	.byte	0x3
	.byte	0x23
	.uleb128 0x180
	.uleb128 0x13
	.4byte	.LASF90
	.byte	0x8
	.2byte	0x1a5
	.4byte	0xe8
	.byte	0x3
	.byte	0x23
	.uleb128 0x190
	.uleb128 0x13
	.4byte	.LASF91
	.byte	0x8
	.2byte	0x1a6
	.4byte	0xe8
	.byte	0x3
	.byte	0x23
	.uleb128 0x194
	.uleb128 0x13
	.4byte	.LASF92
	.byte	0x8
	.2byte	0x1a7
	.4byte	0xe8
	.byte	0x3
	.byte	0x23
	.uleb128 0x198
	.uleb128 0x13
	.4byte	.LASF82
	.byte	0x8
	.2byte	0x1a9
	.4byte	0x5c5
	.byte	0x3
	.byte	0x23
	.uleb128 0x19c
	.uleb128 0x13
	.4byte	.LASF93
	.byte	0x8
	.2byte	0x1bc
	.4byte	0x74
	.byte	0x3
	.byte	0x23
	.uleb128 0x1a8
	.uleb128 0x13
	.4byte	.LASF94
	.byte	0x8
	.2byte	0x1bd
	.4byte	0x5d0
	.byte	0x3
	.byte	0x23
	.uleb128 0x1ac
	.uleb128 0x13
	.4byte	.LASF95
	.byte	0x8
	.2byte	0x1be
	.4byte	0x74
	.byte	0x3
	.byte	0x23
	.uleb128 0x1b0
	.byte	0x0
	.uleb128 0xc
	.4byte	0x6b9
	.4byte	0x7a2
	.uleb128 0xd
	.4byte	0x165
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.4byte	0x94
	.4byte	0x7b2
	.uleb128 0xd
	.4byte	0x165
	.byte	0x1f
	.byte	0x0
	.uleb128 0xc
	.4byte	0x94
	.4byte	0x7c2
	.uleb128 0xd
	.4byte	0x165
	.byte	0x3
	.byte	0x0
	.uleb128 0x11
	.4byte	.LASF96
	.byte	0x8
	.2byte	0x1f6
	.4byte	0x7ce
	.uleb128 0xc
	.4byte	0xc7
	.4byte	0x7de
	.uleb128 0xd
	.4byte	0x165
	.byte	0xf
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF97
	.byte	0x30
	.byte	0x9
	.byte	0x3b
	.4byte	0x815
	.uleb128 0x8
	.4byte	.LASF98
	.byte	0x9
	.byte	0x3d
	.4byte	0x815
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF99
	.byte	0x9
	.byte	0x3f
	.4byte	0xf3
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF83
	.byte	0x9
	.byte	0x44
	.4byte	0x81a
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0x0
	.uleb128 0x15
	.4byte	0x62
	.uleb128 0xc
	.4byte	0xf3
	.4byte	0x82a
	.uleb128 0xd
	.4byte	0x165
	.byte	0x3
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF100
	.byte	0x14
	.byte	0xa
	.byte	0x34
	.4byte	0x87d
	.uleb128 0x8
	.4byte	.LASF101
	.byte	0xa
	.byte	0x35
	.4byte	0x980
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF102
	.byte	0xa
	.byte	0x36
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xf
	.ascii	"vsp\000"
	.byte	0xa
	.byte	0x37
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xf
	.ascii	"vlr\000"
	.byte	0xa
	.byte	0x38
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF103
	.byte	0xa
	.byte	0x39
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF101
	.2byte	0x220
	.byte	0x3
	.byte	0x34
	.4byte	0x980
	.uleb128 0x8
	.4byte	.LASF104
	.byte	0xb
	.byte	0x3e
	.4byte	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF105
	.byte	0xb
	.byte	0x40
	.4byte	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF78
	.byte	0xb
	.byte	0x42
	.4byte	0xf76
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF106
	.byte	0xb
	.byte	0x44
	.4byte	0xf06
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF107
	.byte	0xb
	.byte	0x46
	.4byte	0x980
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF108
	.byte	0xb
	.byte	0x48
	.4byte	0x9d3
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x8
	.4byte	.LASF109
	.byte	0xb
	.byte	0x49
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x8
	.4byte	.LASF110
	.byte	0xb
	.byte	0x4b
	.4byte	0x9d3
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0x8
	.4byte	.LASF111
	.byte	0xb
	.byte	0x4d
	.4byte	0xfe
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0x8
	.4byte	.LASF112
	.byte	0xb
	.byte	0x4f
	.4byte	0x7de
	.byte	0x2
	.byte	0x23
	.uleb128 0x68
	.uleb128 0x8
	.4byte	.LASF113
	.byte	0xb
	.byte	0x51
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x98
	.uleb128 0x8
	.4byte	.LASF114
	.byte	0xb
	.byte	0x53
	.4byte	0xf7c
	.byte	0x3
	.byte	0x23
	.uleb128 0x9c
	.uleb128 0x8
	.4byte	.LASF115
	.byte	0xb
	.byte	0x55
	.4byte	0xc50
	.byte	0x3
	.byte	0x23
	.uleb128 0xb8
	.uleb128 0x8
	.4byte	.LASF116
	.byte	0xb
	.byte	0x58
	.4byte	0x168
	.byte	0x3
	.byte	0x23
	.uleb128 0xbc
	.uleb128 0x8
	.4byte	.LASF117
	.byte	0xb
	.byte	0x5a
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0xc0
	.uleb128 0x8
	.4byte	.LASF118
	.byte	0xb
	.byte	0x5c
	.4byte	0x168
	.byte	0x3
	.byte	0x23
	.uleb128 0xc4
	.uleb128 0x8
	.4byte	.LASF82
	.byte	0xb
	.byte	0x5e
	.4byte	0xbb9
	.byte	0x3
	.byte	0x23
	.uleb128 0xe0
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0x87d
	.uleb128 0x7
	.4byte	.LASF119
	.byte	0x20
	.byte	0xc
	.byte	0x7
	.4byte	0x9bd
	.uleb128 0x8
	.4byte	.LASF120
	.byte	0xc
	.byte	0x8
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF121
	.byte	0xc
	.byte	0x9
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF122
	.byte	0xc
	.byte	0xa
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF123
	.byte	0xc
	.byte	0xb
	.4byte	0x986
	.uleb128 0x2
	.4byte	.LASF124
	.byte	0xd
	.byte	0x31
	.4byte	0xb1
	.uleb128 0x7
	.4byte	.LASF108
	.byte	0x20
	.byte	0xe
	.byte	0x10
	.4byte	0xa34
	.uleb128 0x8
	.4byte	.LASF125
	.byte	0xe
	.byte	0x12
	.4byte	0x9c8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xf
	.ascii	"cpu\000"
	.byte	0xe
	.byte	0x14
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF126
	.byte	0xe
	.byte	0x16
	.4byte	0xa40
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF127
	.byte	0xe
	.byte	0x17
	.4byte	0xfe
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF128
	.byte	0xe
	.byte	0x19
	.4byte	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x8
	.4byte	.LASF18
	.byte	0xe
	.byte	0x1a
	.4byte	0xa46
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.byte	0x0
	.uleb128 0x16
	.byte	0x1
	.4byte	0xa40
	.uleb128 0x17
	.4byte	0xfe
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0xa34
	.uleb128 0x9
	.byte	0x4
	.4byte	0x9d3
	.uleb128 0x7
	.4byte	.LASF129
	.byte	0xc
	.byte	0xf
	.byte	0x4a
	.4byte	0xa83
	.uleb128 0x8
	.4byte	.LASF130
	.byte	0xf
	.byte	0x4c
	.4byte	0xe8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF131
	.byte	0xf
	.byte	0x4e
	.4byte	0x5d0
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF132
	.byte	0xf
	.byte	0x53
	.4byte	0xe8
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF133
	.byte	0xf
	.byte	0x54
	.4byte	0xa4c
	.uleb128 0xb
	.byte	0xc
	.byte	0x10
	.byte	0x20
	.4byte	0xac1
	.uleb128 0xf
	.ascii	"pin\000"
	.byte	0x10
	.byte	0x21
	.4byte	0xa6
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF131
	.byte	0x10
	.byte	0x22
	.4byte	0x5d0
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF132
	.byte	0x10
	.byte	0x23
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF134
	.byte	0x10
	.byte	0x24
	.4byte	0xa8e
	.uleb128 0xb
	.byte	0x4
	.byte	0x10
	.byte	0x3f
	.4byte	0xaf1
	.uleb128 0x8
	.4byte	.LASF135
	.byte	0x10
	.byte	0x40
	.4byte	0x9b
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF131
	.byte	0x10
	.byte	0x41
	.4byte	0x5d0
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF136
	.byte	0x10
	.byte	0x42
	.4byte	0xacc
	.uleb128 0xb
	.byte	0x1c
	.byte	0x10
	.byte	0x48
	.4byte	0xb75
	.uleb128 0x8
	.4byte	.LASF137
	.byte	0x10
	.byte	0x4a
	.4byte	0xb75
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF138
	.byte	0x10
	.byte	0x4c
	.4byte	0xb7b
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF139
	.byte	0x10
	.byte	0x4e
	.4byte	0xb81
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF140
	.byte	0x10
	.byte	0x4f
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF141
	.byte	0x10
	.byte	0x50
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF142
	.byte	0x10
	.byte	0x51
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x8
	.4byte	.LASF143
	.byte	0x10
	.byte	0x52
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x8
	.4byte	.LASF144
	.byte	0x10
	.byte	0x54
	.4byte	0x133
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0xa83
	.uleb128 0x9
	.byte	0x4
	.4byte	0xac1
	.uleb128 0x9
	.byte	0x4
	.4byte	0xaf1
	.uleb128 0x2
	.4byte	.LASF145
	.byte	0x10
	.byte	0x55
	.4byte	0xafc
	.uleb128 0x7
	.4byte	.LASF146
	.byte	0x20
	.byte	0x11
	.byte	0x39
	.4byte	0xbad
	.uleb128 0x8
	.4byte	.LASF147
	.byte	0x11
	.byte	0x3b
	.4byte	0xbb3
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0x18
	.4byte	.LASF205
	.byte	0x1
	.uleb128 0x9
	.byte	0x4
	.4byte	0xbad
	.uleb128 0x10
	.4byte	.LASF148
	.2byte	0x140
	.byte	0x11
	.byte	0x40
	.4byte	0xc29
	.uleb128 0x8
	.4byte	.LASF130
	.byte	0x11
	.byte	0x41
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF149
	.byte	0x11
	.byte	0x42
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF150
	.byte	0x11
	.byte	0x43
	.4byte	0x173
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF151
	.byte	0x11
	.byte	0x44
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF152
	.byte	0x11
	.byte	0x45
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF153
	.byte	0x11
	.byte	0x46
	.4byte	0xc29
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x8
	.4byte	.LASF154
	.byte	0x11
	.byte	0x47
	.4byte	0x450
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.byte	0x0
	.uleb128 0xc
	.4byte	0x94
	.4byte	0xc39
	.uleb128 0xd
	.4byte	0x165
	.byte	0x7
	.byte	0x0
	.uleb128 0xb
	.byte	0x4
	.byte	0x12
	.byte	0x4
	.4byte	0xc50
	.uleb128 0x8
	.4byte	.LASF155
	.byte	0x12
	.byte	0x5
	.4byte	0x815
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0x2
	.4byte	.LASF156
	.byte	0x12
	.byte	0x6
	.4byte	0xc39
	.uleb128 0xb
	.byte	0x8
	.byte	0xb
	.byte	0x2a
	.4byte	0xc80
	.uleb128 0x8
	.4byte	.LASF157
	.byte	0xb
	.byte	0x2b
	.4byte	0x5d0
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xf
	.ascii	"use\000"
	.byte	0xb
	.byte	0x2c
	.4byte	0xa6
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0x0
	.uleb128 0xb
	.byte	0xc
	.byte	0xb
	.byte	0x2e
	.4byte	0xcb3
	.uleb128 0x8
	.4byte	.LASF158
	.byte	0xb
	.byte	0x2f
	.4byte	0x9b
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF159
	.byte	0xb
	.byte	0x30
	.4byte	0xf06
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xf
	.ascii	"use\000"
	.byte	0xb
	.byte	0x31
	.4byte	0xa6
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF106
	.2byte	0x300
	.byte	0x3
	.byte	0x33
	.4byte	0xf06
	.uleb128 0x8
	.4byte	.LASF160
	.byte	0xb
	.byte	0x67
	.4byte	0x5d0
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF85
	.byte	0xb
	.byte	0x69
	.4byte	0xf8c
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF161
	.byte	0xb
	.byte	0x6b
	.4byte	0x133
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF162
	.byte	0xb
	.byte	0x6d
	.4byte	0x133
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF163
	.byte	0xb
	.byte	0x6e
	.4byte	0x100
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.4byte	.LASF164
	.byte	0xb
	.byte	0x6f
	.4byte	0x100
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF165
	.byte	0xb
	.byte	0x70
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x8
	.4byte	.LASF166
	.byte	0xb
	.byte	0x71
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x8
	.4byte	.LASF167
	.byte	0xb
	.byte	0x72
	.4byte	0x74
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x8
	.4byte	.LASF168
	.byte	0xb
	.byte	0x75
	.4byte	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x8
	.4byte	.LASF111
	.byte	0xb
	.byte	0x76
	.4byte	0xfe
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x8
	.4byte	.LASF107
	.byte	0xb
	.byte	0x78
	.4byte	0xf06
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0x8
	.4byte	.LASF169
	.byte	0xb
	.byte	0x79
	.4byte	0xf06
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x8
	.4byte	.LASF170
	.byte	0xb
	.byte	0x7b
	.4byte	0x100
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0x8
	.4byte	.LASF171
	.byte	0xb
	.byte	0x7c
	.4byte	0x133
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0x8
	.4byte	.LASF172
	.byte	0xb
	.byte	0x7f
	.4byte	0xf92
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0x8
	.4byte	.LASF173
	.byte	0xb
	.byte	0x80
	.4byte	0x133
	.byte	0x2
	.byte	0x23
	.uleb128 0x5c
	.uleb128 0x8
	.4byte	.LASF174
	.byte	0xb
	.byte	0x82
	.4byte	0xfa8
	.byte	0x2
	.byte	0x23
	.uleb128 0x5c
	.uleb128 0x8
	.4byte	.LASF175
	.byte	0xb
	.byte	0x8a
	.4byte	0xfae
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0x8
	.4byte	.LASF176
	.byte	0xb
	.byte	0x8b
	.4byte	0xc29
	.byte	0x3
	.byte	0x23
	.uleb128 0x260
	.uleb128 0x8
	.4byte	.LASF177
	.byte	0xb
	.byte	0x8e
	.4byte	0xbb3
	.byte	0x3
	.byte	0x23
	.uleb128 0x280
	.uleb128 0x8
	.4byte	.LASF178
	.byte	0xb
	.byte	0x8f
	.4byte	0xbb3
	.byte	0x3
	.byte	0x23
	.uleb128 0x284
	.uleb128 0x8
	.4byte	.LASF179
	.byte	0xb
	.byte	0x90
	.4byte	0xbb3
	.byte	0x3
	.byte	0x23
	.uleb128 0x288
	.uleb128 0x8
	.4byte	.LASF180
	.byte	0xb
	.byte	0x92
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x28c
	.uleb128 0x8
	.4byte	.LASF181
	.byte	0xb
	.byte	0x93
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x290
	.uleb128 0x8
	.4byte	.LASF182
	.byte	0xb
	.byte	0x95
	.4byte	0xc50
	.byte	0x3
	.byte	0x23
	.uleb128 0x294
	.uleb128 0x8
	.4byte	.LASF101
	.byte	0xb
	.byte	0x97
	.4byte	0xfbe
	.byte	0x3
	.byte	0x23
	.uleb128 0x298
	.uleb128 0x8
	.4byte	.LASF183
	.byte	0xb
	.byte	0x9a
	.4byte	0x168
	.byte	0x3
	.byte	0x23
	.uleb128 0x29c
	.uleb128 0x8
	.4byte	.LASF82
	.byte	0xb
	.byte	0x9c
	.4byte	0xb92
	.byte	0x3
	.byte	0x23
	.uleb128 0x2a0
	.uleb128 0x8
	.4byte	.LASF184
	.byte	0xb
	.byte	0x9e
	.4byte	0xfe
	.byte	0x3
	.byte	0x23
	.uleb128 0x2c0
	.uleb128 0x8
	.4byte	.LASF185
	.byte	0xb
	.byte	0xa1
	.4byte	0x7c2
	.byte	0x3
	.byte	0x23
	.uleb128 0x2c4
	.uleb128 0x8
	.4byte	.LASF186
	.byte	0xb
	.byte	0xa3
	.4byte	0xe8
	.byte	0x3
	.byte	0x23
	.uleb128 0x2d4
	.uleb128 0x8
	.4byte	.LASF187
	.byte	0xb
	.byte	0xa4
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x2d8
	.uleb128 0x8
	.4byte	.LASF188
	.byte	0xb
	.byte	0xa7
	.4byte	0xe8
	.byte	0x3
	.byte	0x23
	.uleb128 0x2dc
	.uleb128 0x8
	.4byte	.LASF189
	.byte	0xb
	.byte	0xa8
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x2e0
	.uleb128 0x8
	.4byte	.LASF190
	.byte	0xb
	.byte	0xab
	.4byte	0x94
	.byte	0x3
	.byte	0x23
	.uleb128 0x2e4
	.uleb128 0x8
	.4byte	.LASF191
	.byte	0xb
	.byte	0xac
	.4byte	0xc7
	.byte	0x3
	.byte	0x23
	.uleb128 0x2e8
	.uleb128 0x8
	.4byte	.LASF192
	.byte	0xb
	.byte	0xad
	.4byte	0xdd
	.byte	0x3
	.byte	0x23
	.uleb128 0x2ea
	.uleb128 0x8
	.4byte	.LASF193
	.byte	0xb
	.byte	0xae
	.4byte	0xdd
	.byte	0x3
	.byte	0x23
	.uleb128 0x2ec
	.uleb128 0x8
	.4byte	.LASF194
	.byte	0xb
	.byte	0xb0
	.4byte	0xe8
	.byte	0x3
	.byte	0x23
	.uleb128 0x2f0
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0xcb3
	.uleb128 0x19
	.byte	0xc
	.byte	0xb
	.byte	0x29
	.4byte	0xf41
	.uleb128 0x1a
	.4byte	.LASF195
	.byte	0xb
	.byte	0x2d
	.4byte	0xc5b
	.uleb128 0x1a
	.4byte	.LASF196
	.byte	0xb
	.byte	0x32
	.4byte	0xc80
	.uleb128 0x1a
	.4byte	.LASF197
	.byte	0xb
	.byte	0x34
	.4byte	0x9b
	.uleb128 0x1a
	.4byte	.LASF198
	.byte	0xb
	.byte	0x35
	.4byte	0x9b
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF172
	.byte	0x20
	.byte	0xb
	.byte	0x1f
	.4byte	0xf76
	.uleb128 0x8
	.4byte	.LASF98
	.byte	0xb
	.byte	0x27
	.4byte	0x9b
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF199
	.byte	0xb
	.byte	0x28
	.4byte	0x9b
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0xf
	.ascii	"u\000"
	.byte	0xb
	.byte	0x36
	.4byte	0xf0c
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0x6b9
	.uleb128 0xc
	.4byte	0x9b
	.4byte	0xf8c
	.uleb128 0xd
	.4byte	0x165
	.byte	0xc
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0x6c5
	.uleb128 0xc
	.4byte	0xfa2
	.4byte	0xfa2
	.uleb128 0xd
	.4byte	0x165
	.byte	0x7
	.byte	0x0
	.uleb128 0x9
	.byte	0x4
	.4byte	0xf41
	.uleb128 0x9
	.byte	0x4
	.4byte	0xb87
	.uleb128 0xc
	.4byte	0x9b
	.4byte	0xfbe
	.uleb128 0xd
	.4byte	0x165
	.byte	0xff
	.byte	0x0
	.uleb128 0xc
	.4byte	0x980
	.4byte	0xfce
	.uleb128 0xd
	.4byte	0x165
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.byte	0x1
	.4byte	.LASF206
	.byte	0x1
	.byte	0x3a
	.byte	0x1
	.4byte	0x62
	.4byte	.LFB109
	.4byte	.LFE109
	.4byte	.LLST0
	.uleb128 0xc
	.4byte	0x9bd
	.4byte	0xff2
	.uleb128 0x1c
	.byte	0x0
	.uleb128 0x1d
	.4byte	.LASF200
	.byte	0x13
	.byte	0x15
	.4byte	0xfff
	.byte	0x1
	.byte	0x1
	.uleb128 0x15
	.4byte	0xfe7
	.uleb128 0x1d
	.4byte	.LASF201
	.byte	0x14
	.byte	0x15
	.4byte	0xbc
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
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
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
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
	.uleb128 0x6
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
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
	.uleb128 0x9
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x13
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x10
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x16
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x17
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x18
	.uleb128 0x13
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x19
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1a
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
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
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
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x1c
	.uleb128 0x21
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.uleb128 0x1d
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
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x17
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x1012
	.4byte	0xfce
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
.LASF118:
	.ascii	"vcpu_dirty_cpumask\000"
.LASF202:
	.ascii	"GNU C 4.4.3\000"
.LASF199:
	.ascii	"notify_vcpu_id\000"
.LASF141:
	.ascii	"maptrack_order\000"
.LASF78:
	.ascii	"vcpu_info\000"
.LASF64:
	.ascii	"arch_shared_info\000"
.LASF87:
	.ascii	"evtchn_mask\000"
.LASF69:
	.ascii	"domid_t\000"
.LASF70:
	.ascii	"vcpu_time_info\000"
.LASF124:
	.ascii	"s_time_t\000"
.LASF130:
	.ascii	"flags\000"
.LASF150:
	.ascii	"guest_vtable\000"
.LASF190:
	.ascii	"scid\000"
.LASF81:
	.ascii	"evtchn_pending_sel\000"
.LASF54:
	.ascii	"failsafe_callback\000"
.LASF8:
	.ascii	"unsigned int\000"
.LASF56:
	.ascii	"trap\000"
.LASF18:
	.ascii	"next\000"
.LASF71:
	.ascii	"version\000"
.LASF135:
	.ascii	"ref_and_flags\000"
.LASF94:
	.ascii	"shutdown_domain\000"
.LASF113:
	.ascii	"vcpu_flags\000"
.LASF148:
	.ascii	"arch_vcpu\000"
.LASF147:
	.ascii	"ioport_caps\000"
.LASF26:
	.ascii	"cpu_user_regs\000"
.LASF176:
	.ascii	"pirq_mask\000"
.LASF191:
	.ascii	"acm_batterylife\000"
.LASF19:
	.ascii	"prev\000"
.LASF119:
	.ascii	"irq_cpu_stat\000"
.LASF102:
	.ascii	"vspsr\000"
.LASF48:
	.ascii	"cpu_sys_regs_t\000"
.LASF138:
	.ascii	"active\000"
.LASF107:
	.ascii	"next_in_list\000"
.LASF93:
	.ascii	"batterylife\000"
.LASF179:
	.ascii	"dma_caps\000"
.LASF134:
	.ascii	"active_grant_entry_t\000"
.LASF43:
	.ascii	"cpu_ext_regs_t\000"
.LASF55:
	.ascii	"arch_vcpu_info\000"
.LASF16:
	.ascii	"uint32_t\000"
.LASF14:
	.ascii	"int8_t\000"
.LASF120:
	.ascii	"__softirq_pending\000"
.LASF149:
	.ascii	"guest_table\000"
.LASF189:
	.ascii	"console_mfn\000"
.LASF65:
	.ascii	"max_pfn\000"
.LASF91:
	.ascii	"wc_sec\000"
.LASF205:
	.ascii	"rangeset\000"
.LASF171:
	.ascii	"rangesets_lock\000"
.LASF200:
	.ascii	"irq_stat\000"
.LASF11:
	.ascii	"long long unsigned int\000"
.LASF74:
	.ascii	"offset\000"
.LASF49:
	.ascii	"vcpu_guest_context\000"
.LASF158:
	.ascii	"remote_port\000"
.LASF72:
	.ascii	"pad0\000"
.LASF76:
	.ascii	"pad1\000"
.LASF101:
	.ascii	"vcpu\000"
.LASF83:
	.ascii	"time\000"
.LASF145:
	.ascii	"grant_table_t\000"
.LASF111:
	.ascii	"sched_priv\000"
.LASF165:
	.ascii	"tot_pages\000"
.LASF50:
	.ascii	"user_regs\000"
.LASF155:
	.ascii	"counter\000"
.LASF167:
	.ascii	"xenheap_pages\000"
.LASF5:
	.ascii	"__u16\000"
.LASF53:
	.ascii	"event_callback\000"
.LASF203:
	.ascii	"xen/asm-offsets.c\000"
.LASF109:
	.ascii	"sleep_tick\000"
.LASF68:
	.ascii	"arch_shared_info_t\000"
.LASF46:
	.ascii	"dacr\000"
.LASF164:
	.ascii	"xenpage_list\000"
.LASF82:
	.ascii	"arch\000"
.LASF99:
	.ascii	"state_entry_time\000"
.LASF131:
	.ascii	"domid\000"
.LASF201:
	.ascii	"jiffies_64\000"
.LASF122:
	.ascii	"__nmi_count\000"
.LASF125:
	.ascii	"expires\000"
.LASF41:
	.ascii	"wcid\000"
.LASF95:
	.ascii	"ticks\000"
.LASF192:
	.ascii	"acm_battery_save_mode\000"
.LASF162:
	.ascii	"page_alloc_lock\000"
.LASF184:
	.ascii	"ssid\000"
.LASF23:
	.ascii	"char\000"
.LASF146:
	.ascii	"arch_domain\000"
.LASF160:
	.ascii	"domain_id\000"
.LASF197:
	.ascii	"pirq\000"
.LASF58:
	.ascii	"reserved10\000"
.LASF59:
	.ascii	"reserved11\000"
.LASF60:
	.ascii	"reserved12\000"
.LASF61:
	.ascii	"reserved13\000"
.LASF62:
	.ascii	"reserved14\000"
.LASF196:
	.ascii	"interdomain\000"
.LASF127:
	.ascii	"data\000"
.LASF183:
	.ascii	"domain_dirty_cpumask\000"
.LASF13:
	.ascii	"uint8_t\000"
.LASF7:
	.ascii	"__u32\000"
.LASF157:
	.ascii	"remote_domid\000"
.LASF21:
	.ascii	"bits\000"
.LASF144:
	.ascii	"lock\000"
.LASF63:
	.ascii	"arch_vcpu_info_t\000"
.LASF103:
	.ascii	"vdacr\000"
.LASF9:
	.ascii	"long long int\000"
.LASF67:
	.ascii	"nmi_reason\000"
.LASF163:
	.ascii	"page_list\000"
.LASF47:
	.ascii	"pidr\000"
.LASF174:
	.ascii	"grant_table\000"
.LASF97:
	.ascii	"vcpu_runstate_info\000"
.LASF92:
	.ascii	"wc_nsec\000"
.LASF166:
	.ascii	"max_pages\000"
.LASF188:
	.ascii	"console_port\000"
.LASF168:
	.ascii	"shutdown_code\000"
.LASF105:
	.ascii	"processor\000"
.LASF85:
	.ascii	"shared_info\000"
.LASF116:
	.ascii	"cpu_affinity\000"
.LASF73:
	.ascii	"freetimer_overflows\000"
.LASF89:
	.ascii	"vdma_mask\000"
.LASF152:
	.ascii	"guest_vstart\000"
.LASF159:
	.ascii	"remote_dom\000"
.LASF204:
	.ascii	"/home/andreiw/src/xen3-arm-tegra/xen/arch/arm\000"
.LASF45:
	.ascii	"cpar\000"
.LASF100:
	.ascii	"cpu_info\000"
.LASF29:
	.ascii	"wr10\000"
.LASF30:
	.ascii	"wr11\000"
.LASF31:
	.ascii	"wr12\000"
.LASF32:
	.ascii	"wr13\000"
.LASF33:
	.ascii	"wr14\000"
.LASF34:
	.ascii	"wr15\000"
.LASF172:
	.ascii	"evtchn\000"
.LASF36:
	.ascii	"wcasf\000"
.LASF170:
	.ascii	"rangesets\000"
.LASF15:
	.ascii	"uint16_t\000"
.LASF66:
	.ascii	"pfn_to_mfn_frame_list_list\000"
.LASF156:
	.ascii	"atomic_t\000"
.LASF77:
	.ascii	"vcpu_time_info_t\000"
.LASF10:
	.ascii	"__u64\000"
.LASF0:
	.ascii	"__s8\000"
.LASF20:
	.ascii	"spinlock_t\000"
.LASF4:
	.ascii	"short int\000"
.LASF140:
	.ascii	"maptrack_head\000"
.LASF75:
	.ascii	"system_time\000"
.LASF24:
	.ascii	"long int\000"
.LASF104:
	.ascii	"vcpu_id\000"
.LASF186:
	.ascii	"store_port\000"
.LASF22:
	.ascii	"cpumask_t\000"
.LASF187:
	.ascii	"store_mfn\000"
.LASF80:
	.ascii	"evtchn_upcall_mask\000"
.LASF88:
	.ascii	"vdma_pending\000"
.LASF142:
	.ascii	"maptrack_limit\000"
.LASF110:
	.ascii	"poll_timer\000"
.LASF17:
	.ascii	"uint64_t\000"
.LASF96:
	.ascii	"xen_domain_handle_t\000"
.LASF182:
	.ascii	"refcnt\000"
.LASF136:
	.ascii	"grant_mapping_t\000"
.LASF153:
	.ascii	"trap_table\000"
.LASF106:
	.ascii	"domain\000"
.LASF35:
	.ascii	"wcssf\000"
.LASF143:
	.ascii	"map_count\000"
.LASF151:
	.ascii	"guest_pstart\000"
.LASF128:
	.ascii	"activated\000"
.LASF42:
	.ascii	"wcon\000"
.LASF198:
	.ascii	"virq\000"
.LASF137:
	.ascii	"shared\000"
.LASF161:
	.ascii	"big_lock\000"
.LASF177:
	.ascii	"iomem_caps\000"
.LASF115:
	.ascii	"pausecnt\000"
.LASF12:
	.ascii	"long unsigned int\000"
.LASF1:
	.ascii	"__u8\000"
.LASF114:
	.ascii	"virq_to_evtchn\000"
.LASF180:
	.ascii	"domain_flags\000"
.LASF169:
	.ascii	"next_in_hashbucket\000"
.LASF139:
	.ascii	"maptrack\000"
.LASF3:
	.ascii	"unsigned char\000"
.LASF126:
	.ascii	"function\000"
.LASF154:
	.ascii	"guest_context\000"
.LASF112:
	.ascii	"runstate\000"
.LASF27:
	.ascii	"cpu_user_regs_t\000"
.LASF79:
	.ascii	"evtchn_upcall_pending\000"
.LASF37:
	.ascii	"wcgr0\000"
.LASF28:
	.ascii	"cpu_ext_regs\000"
.LASF39:
	.ascii	"wcgr2\000"
.LASF25:
	.ascii	"list_head\000"
.LASF98:
	.ascii	"state\000"
.LASF52:
	.ascii	"sys_regs\000"
.LASF57:
	.ascii	"spsr\000"
.LASF129:
	.ascii	"grant_entry\000"
.LASF132:
	.ascii	"frame\000"
.LASF84:
	.ascii	"vcpu_info_t\000"
.LASF195:
	.ascii	"unbound\000"
.LASF51:
	.ascii	"ext_regs\000"
.LASF2:
	.ascii	"signed char\000"
.LASF175:
	.ascii	"pirq_to_evtchn\000"
.LASF121:
	.ascii	"__local_irq_count\000"
.LASF193:
	.ascii	"acm_shutdown\000"
.LASF6:
	.ascii	"short unsigned int\000"
.LASF185:
	.ascii	"handle\000"
.LASF206:
	.ascii	"main\000"
.LASF181:
	.ascii	"vm_assist\000"
.LASF133:
	.ascii	"grant_entry_t\000"
.LASF44:
	.ascii	"cpu_sys_regs\000"
.LASF173:
	.ascii	"evtchn_lock\000"
.LASF108:
	.ascii	"timer\000"
.LASF117:
	.ascii	"nmi_addr\000"
.LASF86:
	.ascii	"evtchn_pending\000"
.LASF38:
	.ascii	"wcgr1\000"
.LASF178:
	.ascii	"irq_caps\000"
.LASF123:
	.ascii	"irq_cpustat_t\000"
.LASF40:
	.ascii	"wcgr3\000"
.LASF90:
	.ascii	"wc_version\000"
.LASF194:
	.ascii	"evtchn_count\000"
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
