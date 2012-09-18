/*
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 * Copyright (C) 2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <xen/config.h>
#include <xen/errno.h>
#include <xen/lib.h>
#include <xen/mm.h>
#include <xen/smp.h>
#include <xen/cpumask.h>
#include <xen/percpu.h>
#include <asm/irq.h>

DEFINE_PER_CPU(void *, fiq_stack);

extern unsigned char fiq_glue, fiq_glue_end;
extern void fiq_glue_setup(void *func, void *data, void *sp);

static void fiq_set_handler(void *handler)
{
	unsigned long *fiq_slot = (unsigned long *)
		(VECTORS_BASE + (exception_vectors_jump_fiq -
				 exception_vectors_table));
	*fiq_slot = (u32) handler;
	cpu_coherent_range(fiq_slot, fiq_slot + 1);
}

static void fiq_setup_helper(void *info)
{
	struct fiq_handler *handler = info;

	fiq_glue_setup(handler->fiq, handler,
		       __get_cpu_var(fiq_stack) + STACK_SIZE);
}

int fiq_register_handler(struct fiq_handler *handler)
{
	int ret;
	int cpu;

	if (!handler || !handler->fiq)
		return -EINVAL;

	for_each_cpu(cpu) {
		void *stack;
		stack = pages_m_alloc(STACK_ORDER);
		if (!stack) {
			ret = -ENOMEM;
			goto err_alloc_fiq_stack;
		}
		per_cpu(fiq_stack, cpu) = stack;
	}

	on_each_cpu(fiq_setup_helper, handler, true, true);
	fiq_set_handler(&fiq_glue);
	return 0;

err_alloc_fiq_stack:
	for_each_cpu(cpu) {
		pages_m_free(per_cpu(fiq_stack, cpu), STACK_ORDER);
		per_cpu(fiq_stack, cpu) = NULL;
	}
err_busy:
	return ret;
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
