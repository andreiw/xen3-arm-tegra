# -*- mode: Makefile; -*-

# A debug build of Xen and tools?
debug ?= n

XEN_COMPILE_ARCH    ?= $(shell uname -m | sed -e s/i.86/x86_32/)
XEN_TARGET_ARCH     ?= arm
XEN_TARGET_SUBARCH  ?= imx21
XEN_TARGET_X86_PAE  ?= n

LINUX_ROOT=/home/nirvana/cc_root/nirvana_OSV_mirage_linux_int/OSV_Mirage/linux

# Tools to run on system hosting the build
HOSTCC     = gcc
HOSTCFLAGS = -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

#
# Cross Tool chain configuration
#
CROSS_COMPILE	?= arm-eabi-

#
# Target Configuration for Xen-ARM
#

AS         = $(CROSS_COMPILE)as
LD         = $(CROSS_COMPILE)ld
CC         = $(CROSS_COMPILE)gcc
CPP        = $(CROSS_COMPILE)gcc -E
AR         = $(CROSS_COMPILE)ar
RANLIB     = $(CROSS_COMPILE)ranlib
NM         = $(CROSS_COMPILE)nm
STRIP      = $(CROSS_COMPILE)strip
OBJCOPY    = $(CROSS_COMPILE)objcopy
OBJDUMP    = $(CROSS_COMPILE)objdump

DISTDIR     ?= $(XEN_ROOT)/dist
DESTDIR		?= $(DISTDIR)/install

INSTALL      = install
INSTALL_DIR  = $(INSTALL) -d -m0755
INSTALL_DATA = $(INSTALL) -m0644
INSTALL_PROG = $(INSTALL) -m0755

ifneq ($(debug),y)
# Optimisation flags are overridable
CFLAGS    ?= -Os -fomit-frame-pointer
CFLAGS    += -DNDEBUG
else
CFLAGS    += -g -Os -fomit-frame-pointer
endif

ifeq ($(XEN_TARGET_SUBARCH),ssdtv)
CFLAGS += -march=armv6k -mtune=arm1136j-s
endif

ifneq ($(EXTRA_PREFIX),)
EXTRA_INCLUDES += $(EXTRA_PREFIX)/include
EXTRA_LIB += $(EXTRA_PREFIX)/$(LIBDIR)
endif

test-gcc-flag = $(shell $(1) -v --help 2>&1 | grep -q " $(2) " && echo $(2))

HOSTCFLAGS += $(call test-gcc-flag,$(HOSTCC),-Wdeclaration-after-statement)
CFLAGS     += $(call test-gcc-flag,$(CC),-Wdeclaration-after-statement)

LDFLAGS += $(foreach i, $(EXTRA_LIB), -L$(i)) 
CFLAGS += $(foreach i, $(EXTRA_INCLUDES), -I$(i))

# Choose the best mirror to download linux kernel
KERNEL_REPO = http://www.kernel.org

# If ACM_SECURITY = y, then the access control module is compiled
# into Xen and the policy type can be set by the boot policy file
#        y - Build the Xen ACM framework
#        n - Do not build the Xen ACM framework
ACM_SECURITY ?= n

# If ACM_SECURITY = y and no boot policy file is installed,
# then the ACM defaults to the security policy set by
# ACM_DEFAULT_SECURITY_POLICY
# Supported models are:
#	ACM_NULL_POLICY
#	ACM_CHINESE_WALL_POLICY
#	ACM_SIMPLE_TYPE_ENFORCEMENT_POLICY
#	ACM_CHINESE_WALL_AND_SIMPLE_TYPE_ENFORCEMENT_POLICY
ACM_DEFAULT_SECURITY_POLICY ?= ACM_NULL_POLICY

# Optional components
XENSTAT_XENTOP ?= y

VTPM_TOOLS ?= n

-include $(XEN_ROOT)/.config
