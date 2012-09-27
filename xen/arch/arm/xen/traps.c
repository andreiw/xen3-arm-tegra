/*
 * traps.c
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of
 * Xen on ARM and the associated access control.
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

#include <xen/kernel.h>
#include <xen/config.h>
#include <xen/linkage.h>
#include <xen/compile.h>
#include <xen/domain_page.h>
#include <xen/sched.h>
#include <xen/console.h>
#include <xen/mm.h>
#include <xen/irq.h>
#include <xen/symbols.h>
#include <asm/current.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/system.h>

/* ARM ARM ARMv7 p. 1396. */
#define DFSR_FS(x) (((x) & 7) | (((x) & 0x400) >> 10))
#define DFSR_DOM(x) (((x) & 0xf0) >> 4)
#define DFSR_W(x) (((x) & 0x800) >> 11)
#define DFSR_E(x) (((x) & 0x1000) >> 12)

#define DFSR_ALIGN  (1)
#define DFSR_ICACHE (4)
#define DFSR_TLE1   (12)
#define DFSR_TLE2   (14)
#define DFSR_TLS1   (28)
#define DFSR_TLS2   (30)
#define DFSR_TF_S   (5)
#define DFSR_TF_P   (7)
#define DFSR_AF_S   (3)
#define DFSR_AF_P   (6)
#define DFSR_DF_S   (9)
#define DFSR_DF_P   (11)
#define DFSR_PF_S   (13)
#define DFSR_PF_P   (15)
#define DFSR_DE     (2)
#define DFSR_SEA    (8)
#define DFSR_MASPE  (25)
#define DFSR_AEA    (22)

static const char *processor_modes[] = {
	"USER_26", "FIQ_26" , "IRQ_26" , "SVC_26" , "UK4_26" , "UK5_26" , "UK6_26" , "UK7_26" ,
	"UK8_26" , "UK9_26" , "UK10_26", "UK11_26", "UK12_26", "UK13_26", "UK14_26", "UK15_26",
	"USER_32", "FIQ_32" , "IRQ_32" , "SVC_32" , "UK4_32" , "UK5_32" , "UK6_32" , "ABT_32" ,
	"UK8_32" , "UK9_32" , "UK10_32", "UND_32" , "UK12_32", "UK13_32", "UK14_32", "SYS_32"
};

asmlinkage void __div0(void)
{
        panic("Division by zero in kernel.\n");
}

int fixup_exception(struct cpu_context *regs)
{
	const struct extable_entry *fixup;

	fixup = search_extable(regs->pc);
	if (fixup)
		regs->pc = fixup->fixup;

	return fixup != NULL;
}

void show_registers(struct cpu_context *ctx)
{
	unsigned long flags = condition_codes(ctx);

	printk("CPU: %d\n", smp_processor_id());
	printk("pc : [<%08lx>] \n", ctx->pc);
	printk("slr: %08lx  ssp: %08lx\n", ctx->slr, ctx->ssp);
	printk("ulr: %08lx  usp: %08lx\n", ctx->ulr, ctx->usp);
	printk("ip : %08lx  fp : %08lx\n", ctx->r12, ctx->r11);
	printk("r10: %08lx  r9 : %08lx  r8 : %08lx\n",
		ctx->r10, ctx->r9, ctx->r8);
	printk("r7 : %08lx  r6 : %08lx  r5 : %08lx  r4 : %08lx\n",
		ctx->r7, ctx->r6, ctx->r5, ctx->r4);
	printk("r3 : %08lx  r2 : %08lx  r1 : %08lx  r0 : %08lx\n",
		ctx->r3, ctx->r2, ctx->r1, ctx->r0);
	printk("Flags: %c%c%c%c",
		flags & PSR_N_BIT ? 'N' : 'n',
		flags & PSR_Z_BIT ? 'Z' : 'z',
		flags & PSR_C_BIT ? 'C' : 'c',
		flags & PSR_V_BIT ? 'V' : 'v');
	printk("  IRQs o%s  FIQs o%s  Mode %s%s\n",
		interrupts_enabled(ctx) ? "n" : "ff",
		fast_interrupts_enabled(ctx) ? "n" : "ff",
		processor_modes[processor_mode(ctx)],
		thumb_mode(ctx) ? " (T)" : "");

}

static int verify_stack(unsigned long sp)
{
	if (sp < 0xFF000000) {
		return -EFAULT;
	}

	return 0;
}

static void backtrace(struct cpu_context *ctx)
{
	int ok = 1;
	unsigned long fp;

	printk("Backtrace: ");
	fp = ctx->r12;
	if(!fp) {
		printk("no frame pointer");
		ok = 0;
	}
	else if (verify_stack(fp)) {
		printk("invalid frame pointer 0x%08x", fp);
		ok = 0;
	}
#if 0
	else if (fp < (unsigned long)end_of_stak(tsk))
		printk("frame pointer underflow");
#endif
	printk("\n");

	/* if (ok) */
	/* 	c_backtrace(fp, ctx->spsr); */
}

static void unrecoverable_fault(const char *str, int err, struct domain *d, struct cpu_context *ctx)
{
}

int is_kernel_text(unsigned long addr)
{
	extern char _stext, _etext;
	if (addr >= (unsigned long) &_stext &&
	    addr <= (unsigned long) &_etext)
	    return 1;
	return 0;
}

unsigned long kernel_text_end(void)
{
	extern char _etext;
	return (unsigned long) &_etext;
}

long do_set_callbacks(unsigned long event, unsigned long failsafe)
{
	struct vcpu *v = (struct vcpu *)current;

	if ((event < HYPERVISOR_VIRT_START) && (failsafe < HYPERVISOR_VIRT_START)) {
		v->arch.guest_context.event_callback    = event;
		v->arch.guest_context.failsafe_callback = failsafe;

		return 0;
	}

	printk("Invalid hypervisor callback address = 0x%x, 0x%x\n", event, failsafe);

	return -EINVAL;

}

asmlinkage void do_prefetch_abort(u32 fsr, u32 far, struct cpu_context *regs)
{
	panic("Prefetch abort %08x at 0x%x!\n", fsr, far);
	while(1);
}

asmlinkage void do_data_abort(u32 fsr, u32 far, struct cpu_context *regs)
{
   show_registers(regs);

	panic("Data abort %08x at 0x%x!\n", fsr, far);
	while(1);
}

asmlinkage void do_undefined_instruction(unsigned long pc, struct cpu_context *regs)
{
	panic("Undefined instruction!\n");
	while(1);
}

long do_set_trap_table(GUEST_HANDLE(trap_info_t) traps)
{
	int i;
	struct trap_info ti;
	unsigned long *trap_table = current->arch.trap_table;

	if ( guest_handle_is_null(traps) )
		goto failed;

	for ( i = 0; i < TRAP_TABLE_ENTRIES; i++ ) {
		if ( copy_from_guest(&ti, traps, 1) )
			goto failed;

		if ( ti.address == 0 )
			goto failed;

		trap_table[i] = ti.address;

		guest_handle_add_offset(traps, 1);
	}

	if ( i != TRAP_TABLE_ENTRIES )
		goto failed;

	return 0;

failed:
	memset(trap_table, 0, 8 * sizeof(*trap_table));

	return -EFAULT;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
