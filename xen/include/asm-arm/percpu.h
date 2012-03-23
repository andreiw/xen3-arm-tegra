#ifndef __ARM_PERCPU_H__
#define __ARM_PERCPU_H__

#include <xen/autoconf.h>

#ifdef CONFIG_SMP
#define PERCPU_SHIFT	13
#define PERCPU_SIZE		(1UL << PERCPU_SHIFT)

/* Separate out the type, so (int[3], foo) works. */
#define DEFINE_PER_CPU(type, name)			    \
	__attribute__((__section__(".data.percpu")))        \
	__typeof__(type) per_cpu__##name

/* FIXME: RELOC_HIDE is not defined. */

/* var is in discarded region: offset to particular copy we want */
#define per_cpu(var, cpu)						\
	(*RELOC_HIDE(&per_cpu__##var, ((unsigned int)(cpu))<< PERCPU_SHIFT))
#else
#define DEFINE_PER_CPU(type, name)		\
	__typeof__(type) per_cpu__##name
#define per_cpu(var, cpu) (per_cpu__##var)
#endif /* CONFIG_SMP */

#define __get_cpu_var(var)		per_cpu(var, smp_processor_id())
#define DECLARE_PER_CPU(type, name) extern __typeof__(type) per_cpu__##name

#endif /* __ARM_PERCPU_H__ */
