/*
 * fiqdb.c
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *
 * Based on Android fiq_debugger.c, Copyright (C) 2008 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <xen/types.h>
#include <xen/errno.h>
#include <xen/lib.h>
#include <xen/compile.h>
#include <asm/fiqdb.h>
#include <asm/irq.h>
#include <asm/stacktrace.h>

#define DEBUG_MAX 64

static struct platform_fiqdb *pfiqdb;
static char debug_buf[DEBUG_MAX];
static int debug_count;
static unsigned debug_indent;

struct fiq_regs {
	u32 r0;
	u32 r1;
	u32 r2;
	u32 r3;
	u32 r4;
	u32 r5;
	u32 r6;
	u32 r7;
	u32 r8;
	u32 r9;
	u32 r10;
	u32 fp;
	u32 ip;
	u32 sp;
	u32 lr;
	u32 pc;
	u32 cpsr;
	u32 spsr;
};

static void fiqdb_puts(char *s)
{
	static unsigned col = 0;
	unsigned c;
	unsigned i;

	if (!col) {
		i = debug_indent;
		while (i--)
			pfiqdb->putc(' ');
	}

	while ((c = *s++)) {
		if (c == '\n') {
			pfiqdb->putc('\r');
			col = 0;
		}
		pfiqdb->putc(c);
	}
}

static void fiqdb_printf(const char *fmt, ...)
{
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	fiqdb_puts(buf);
}

static char *mode_name(u32 cpsr)
{
	switch (cpsr & PSR_MODE_MASK) {
	case PSR_MODE_USR: return "USR";
	case PSR_MODE_FIQ: return "FIQ";
	case PSR_MODE_IRQ: return "IRQ";
	case PSR_MODE_SVC: return "SVC";
	case PSR_MODE_ABT: return "ABT";
	case PSR_MODE_UND: return "UND";
	case PSR_MODE_SYS: return "SYS";
	default: return "???";
	}
}

static void fiqdb_prompt(void)
{
	fiqdb_puts("fiqdb> ");
}

static void fiqdb_regs(struct fiq_regs *regs)
{
	fiqdb_printf("r0 %08x  r1 %08x  r2  %08x  r3 %08x\n",
		     regs->r0, regs->r1, regs->r2, regs->r3);
	fiqdb_printf("r4 %08x  r5 %08x  r6  %08x  r7 %08x\n",
		     regs->r4, regs->r5, regs->r6, regs->r7);
	fiqdb_printf("r8 %08x  r9 %08x  r10 %08x  fp %08x\n",
		     regs->r8, regs->r9, regs->r10, regs->fp);
	fiqdb_printf("ip %08x  sp %08x  lr  %08x  pc %08x\n",
		     regs->ip, regs->sp, regs->lr, regs->pc);
	fiqdb_printf("cpsr %08x mode %s\n",
		     regs->cpsr, mode_name(regs->cpsr));
	fiqdb_printf("spsr %08x mode %s\n", regs->spsr,
		     mode_name(regs->spsr));
}

struct mode_regs {
	unsigned long sp_svc;
	unsigned long lr_svc;
	unsigned long spsr_svc;

	unsigned long sp_abt;
	unsigned long lr_abt;
	unsigned long spsr_abt;

	unsigned long sp_und;
	unsigned long lr_und;
	unsigned long spsr_und;

	unsigned long sp_irq;
	unsigned long lr_irq;
	unsigned long spsr_irq;

	unsigned long r8_fiq;
	unsigned long r9_fiq;
	unsigned long r10_fiq;
	unsigned long r11_fiq;
	unsigned long r12_fiq;
	unsigned long sp_fiq;
	unsigned long lr_fiq;
	unsigned long spsr_fiq;
};

static void __naked get_mode_regs(struct mode_regs *regs)
{
	asm volatile (
	"mrs	r1, cpsr\n"
	"msr	cpsr_c, #0xd3 @(SVC_MODE | PSR_I_BIT | PSR_F_BIT)\n"
	"stmia	r0!, {r13 - r14}\n"
	"mrs	r2, spsr\n"
	"msr	cpsr_c, #0xd7 @(ABT_MODE | PSR_I_BIT | PSR_F_BIT)\n"
	"stmia	r0!, {r2, r13 - r14}\n"
	"mrs	r2, spsr\n"
	"msr	cpsr_c, #0xdb @(UND_MODE | PSR_I_BIT | PSR_F_BIT)\n"
	"stmia	r0!, {r2, r13 - r14}\n"
	"mrs	r2, spsr\n"
	"msr	cpsr_c, #0xd2 @(IRQ_MODE | PSR_I_BIT | PSR_F_BIT)\n"
	"stmia	r0!, {r2, r13 - r14}\n"
	"mrs	r2, spsr\n"
	"msr	cpsr_c, #0xd1 @(FIQ_MODE | PSR_I_BIT | PSR_F_BIT)\n"
	"stmia	r0!, {r2, r8 - r14}\n"
	"mrs	r2, spsr\n"
	"stmia	r0!, {r2}\n"
	"msr	cpsr_c, r1\n"
	"bx	lr\n");
}

static void fiqdb_xregs(struct fiq_regs *regs)
{
	struct mode_regs mode_regs;

	fiqdb_regs(regs);
	get_mode_regs(&mode_regs);
	fiqdb_printf("svc: sp %08x  lr %08x  spsr %08x  mode %s\n",
		     mode_regs.sp_svc, mode_regs.lr_svc,
		     mode_regs.spsr_svc, mode_name(mode_regs.spsr_svc));
	fiqdb_printf("abt: sp %08x  lr %08x  spsr %08x  mode %s\n",
		     mode_regs.sp_abt, mode_regs.lr_abt,
		     mode_regs.spsr_abt, mode_name(mode_regs.spsr_abt));
	fiqdb_printf("und: sp %08x  lr %08x  spsr %08x  mode %s\n",
		     mode_regs.sp_und, mode_regs.lr_und,
		     mode_regs.spsr_und, mode_name(mode_regs.spsr_und));
	fiqdb_printf("irq: sp %08x  lr %08x  spsr %08x  mode %s\n",
		     mode_regs.sp_irq, mode_regs.lr_irq,
		     mode_regs.spsr_irq, mode_name(mode_regs.spsr_irq));
	fiqdb_printf("fiq: r8 %08x  r9 %08x  r10  %08x  r11  %08x r12 %08x\n",
		     mode_regs.r8_fiq, mode_regs.r9_fiq,
		     mode_regs.r10_fiq, mode_regs.r11_fiq,
		     mode_regs.r12_fiq);
	fiqdb_printf("fiq: sp %08x  lr %08x  spsr %08x  mode %s\n",
		     mode_regs.sp_fiq, mode_regs.lr_fiq,
		     mode_regs.spsr_fiq, mode_name(mode_regs.spsr_fiq));
}

static int fiqdb_bt_report(struct stackframe *frame, void *d)
{
	unsigned int *depth = d;

	if (*depth) {
		fiqdb_printf("pc %08x sp %08x fp %08x\n",
			     frame->pc, frame->sp, frame->fp);
		*depth--;
		return 0;
	}
	fiqdb_printf("...\n");
	return *depth == 0;
}

static void fiqdb_bt(struct fiq_regs *regs,
		     unsigned int depth, void *ssp)
{
	unsigned int d = depth;

	struct stackframe frame;
	frame.fp = regs->fp;
	frame.sp = regs->sp;
	frame.pc = regs->pc;
	if (regs->cpsr & PSR_MODE_MASK == PSR_MODE_USR) {
		fiqdb_printf("Not going to try bt'ing USR stack\n");
		return;
	}

	walk_stackframe(&frame, fiqdb_bt_report, &d);
}

static void fiqdb_exec(char *command,
		       struct fiq_regs *regs,
		       void *svc_sp)
{
	debug_indent++;

	if (!strcmp(command, "bt")) {
		fiqdb_bt(regs, 100, svc_sp);
	} else if (!strcmp(command, "regs")) {
		fiqdb_regs(regs);
	} else if (!strcmp(command, "xregs")) {
		fiqdb_xregs(regs);
	} else if (!strcmp(command, "version")) {
		fiqdb_printf("Xen version %d.%d%s (%s@%s) (%s) %s\n",
			     XEN_VERSION, XEN_SUBVERSION,
			     XEN_EXTRAVERSION,
			     XEN_COMPILE_BY, XEN_COMPILE_DOMAIN,
			     XEN_COMPILER, XEN_COMPILE_DATE);
		fiqdb_printf("Platform: %s\n", XEN_PLATFORM);
		fiqdb_printf("GIT SHA: %s\n", XEN_CHANGESET);
	} else {
		fiqdb_printf("FIQ debugger commands:\n");
		fiqdb_printf(" bt         backtrace\n");
		fiqdb_printf(" regs       register dump\n");
		fiqdb_printf(" xregs      extended register dump\n");
		fiqdb_printf(" version    kernel version\n");
	}
	debug_indent--;
}

static void handle_fiq(struct fiq_handler *h,
		       struct fiq_regs *regs,
		       void *svc_sp)
{
	int c;
	static int last_c;
	int count = 0;

	/* TODO: Make me SMP safe! */
	while ((c = pfiqdb->getc()) != FIQDB_NO_CHAR) {
		count++;

		if ((c >= ' ') && (c < 127)) {
			if (debug_count < (DEBUG_MAX - 1)) {
				debug_buf[debug_count++] = c;
				pfiqdb->putc(c);
			}
		} else if ((c == 8) || (c == 127)) {
			if (debug_count > 0) {
				debug_count--;
				pfiqdb->putc(8);
				pfiqdb->putc(' ');
				pfiqdb->putc(8);
			}
		} else if (c == 13 || c == 10) {
			if (c == '\r' || (c == '\n' && last_c != '\r')) {
				pfiqdb->putc('\r');
				pfiqdb->putc('\n');
			}
			if (debug_count) {
				debug_buf[debug_count] = 0;
				debug_count = 0;
				fiqdb_exec(debug_buf, regs, svc_sp);
			}
			fiqdb_prompt();
		}
		last_c = c;
	}

	pfiqdb->flush();
}

struct fiq_handler fiq_handler = {
	.fiq = handle_fiq,
};

/*
 * Initializes the FIQ debugger.
 */
int fiqdb_init(void)
{
	int status;

	if (!pfiqdb) {
		printk("FIQ debugger not enabled\n");
		return -ENXIO;
        }
	status = pfiqdb->init();
	if (status)
		return status;

	debug_count = 0;
	debug_indent = 0;
	memset(debug_buf, 0, sizeof(debug_buf));

	fiq_register_handler(&fiq_handler);
	local_fiq_enable();
	printk("FIQ debugger enabled\n");
	return 0;
}

/*
 * Registers platform FIQDB operations with the debugger.
 */
void fiqdb_register(struct platform_fiqdb *platform_fiqdb)
{
	pfiqdb = platform_fiqdb;
}

/*
 * Called from panic().
 */
void debugger_trap_immediate(void)
{

	/*
	 * FIQ could be disabled.
	 */
	local_fiq_enable();
	fiqdb_prompt();
	while(1);
}
