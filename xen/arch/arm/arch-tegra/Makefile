include $(BASEDIR)/Rules.mk

obj-y += platform.o
obj-y += irq.o
obj-y += time.o
obj-y += debug.o
obj-y += uart.o
obj-y += delay.o
obj-y += tegra2_clocks.o
obj-$(CONFIG_TEGRA_FIQDB_SUPPORT) += fiqdb_support.o

include $(BASEDIR)/Post.mk
