include $(BASEDIR)/Rules.mk

obj-y := xensetup.o mm.o irq.o fiq.o arch_domain.o domain_page.o time.o domain_build.o traps.o dom0_ops.o physdev.o cache.o tlb.o entry.o hypercalls.o fcse.o extable.o shutdown.o vfp.o

obj-$(CONFIG_CPU_ARMV7)			+= core-armv7.o
obj-$(CONFIG_CPU_COPYPAGE_V6)	+= copypage-v6.o
obj-$(CONFIG_CPU_TLB_V7)		+= tlb-v7.o

include $(BASEDIR)/Post.mk
