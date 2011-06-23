
#
# If you change any of these configuration options then you must
# 'make clean' before rebuilding.
#
verbose     ?= n
perfc       ?= n
perfc_arrays?= n
crash_debug ?= n

# Hardcoded configuration implications and dependencies.
# Do this is a neater way if it becomes unwieldy.
ifeq ($(debug),y)
verbose := y
endif
ifeq ($(perfc_arrays),y)
perfc := y
endif

XEN_ROOT=$(BASEDIR)/..
export XEN_ROOT
include $(XEN_ROOT)/Config.mk

# Set ARCH/SUBARCH appropriately.
override COMPILE_SUBARCH := $(XEN_COMPILE_ARCH)
override TARGET_SUBARCH  := $(XEN_TARGET_SUBARCH)
override COMPILE_ARCH    := $(patsubst x86%,x86,$(XEN_COMPILE_ARCH))
override TARGET_ARCH     := $(patsubst x86%,x86,$(XEN_TARGET_ARCH))

TARGET  := $(BASEDIR)/xen
HDRS    := $(wildcard $(BASEDIR)/include/xen/*.h)
HDRS    += $(wildcard $(BASEDIR)/include/public/*.h)
ifeq ($(CONFIG_VMM_SECURITY), y)
HDRS    += $(wildcard $(BASEDIR)/include/security/crypto/*.h)
endif
HDRS    += $(wildcard $(BASEDIR)/include/asm-$(TARGET_ARCH)/*.h)
HDRS    += $(wildcard $(BASEDIR)/include/asm-$(TARGET_ARCH)/arch/*.h)
HDRS    += $(wildcard $(BASEDIR)/include/asm-$(TARGET_ARCH)/hvm/*.h)
HDRS    += $(wildcard $(BASEDIR)/include/asm-$(TARGET_ARCH)/hvm/svm/*.h)
HDRS    += $(wildcard $(BASEDIR)/include/asm-$(TARGET_ARCH)/hvm/vmx/*.h)
# Do not depend on auto-generated header files.
HDRS    := $(subst $(BASEDIR)/include/asm-$(TARGET_ARCH)/asm-offsets.h,,$(HDRS))
HDRS    := $(subst $(BASEDIR)/include/xen/banner.h,,$(HDRS))
HDRS    := $(subst $(BASEDIR)/include/xen/compile.h,,$(HDRS))

include $(BASEDIR)/arch/$(TARGET_ARCH)/Rules.mk

# Note that link order matters!
ALL_OBJS-y               += $(BASEDIR)/common/built_in.o
ALL_OBJS-y               += $(BASEDIR)/arch/$(TARGET_ARCH)/built_in.o
ALL_OBJS-$(CONFIG_VMM_SECURITY_ACM)  += $(BASEDIR)/security/acm/built_in.o
ALL_OBJS-y               += $(BASEDIR)/drivers/built_in.o
#ALL_OBJS-y 		 += $(BASEDIR)/security/crypto/built_in.o
#ALL_OBJS-y		 += $(BASEDIR)/security/ssm-xen/built_in.o

CFLAGS-y               += -g -D__XEN__ 
CFLAGS-$(ACM_SECURITY) += -DACM_SECURITY
CFLAGS-$(verbose)      += -DVERBOSE
CFLAGS-$(crash_debug)  += -DCRASH_DEBUG
CFLAGS-$(perfc)        += -DPERF_COUNTERS
CFLAGS-$(perfc_arrays) += -DPERF_ARRAYS
#CFLAGS-$(CONFIG_VMM_SECURITY) += -DCONFIG_VMM_SECURITY

CFLAGS-y               += -D__KERNEL__
CFLAGS-y               += 

ifneq ($(max_phys_cpus),)
CFLAGS-y               += -DMAX_PHYS_CPUS=$(max_phys_cpus)
endif

AFLAGS-y               += -D__ASSEMBLY__ 

ALL_OBJS := $(ALL_OBJS-y)
CFLAGS   := $(strip $(CFLAGS) $(CFLAGS-y))
AFLAGS   := $(strip $(AFLAGS) $(AFLAGS-y))

%.o: %.c $(HDRS) Makefile
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HDRS) Makefile
	@$(CC) $(CFLAGS) $(AFLAGS) -c $< -o $@

