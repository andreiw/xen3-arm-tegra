#ifndef __ASM_CPU_DOMAIN_H
#define __ASM_CPU_DOMAIN_H

#define DOMAIN_HYPERVISOR	0
#define DOMAIN_KERNEL		1
#define DOMAIN_SUPERVISOR	1
#define DOMAIN_IO			2
#define DOMAIN_USER			3
#define DOMAIN_METASET		4

/*
 * Domain types
 */
#define DOMAIN_NOACCESS			0
#define DOMAIN_CLIENT			1
#define DOMAIN_MANAGER			3

#define DOMAIN_VALUE(dom,type)	((type) << (2*(dom)))

#define DOMAIN_HYPERVISOR_VALUE					\
	(DOMAIN_VALUE(DOMAIN_HYPERVISOR,DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_KERNEL, DOMAIN_CLIENT) |		\
	 DOMAIN_VALUE(DOMAIN_IO, DOMAIN_CLIENT) |		\
	 DOMAIN_VALUE(DOMAIN_USER,DOMAIN_CLIENT))

#define DOMAIN_SUPERVISOR_VALUE
#define DOMAIN_IO_VALUE

#define DOMAIN_KERNEL_VALUE					\
	(DOMAIN_VALUE(DOMAIN_HYPERVISOR, DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_KERNEL, DOMAIN_MANAGER) |		\
	 DOMAIN_VALUE(DOMAIN_IO, DOMAIN_MANAGER) | 		\
	 DOMAIN_VALUE(DOMAIN_USER, DOMAIN_CLIENT))		\

#define DOMAIN_USER_VALUE					\
	(DOMAIN_VALUE(DOMAIN_HYPERVISOR, DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_KERNEL,   DOMAIN_CLIENT) |		\
	 DOMAIN_VALUE(DOMAIN_IO,       DOMAIN_CLIENT) |	\
	 DOMAIN_VALUE(DOMAIN_USER,     DOMAIN_CLIENT))

#ifndef __ASSEMBLY__
#define isb() __asm__ __volatile__ ("" : : : "memory")

#define set_domain(x)					\
	do {								\
	__asm__ __volatile__(				\
	"mcr	p15, 0, %0, c3, c0"			\
	  : : "r" (x));						\
	isb();								\
	} while (0)
#endif
#endif /* !__ASSEMBLY__ */
