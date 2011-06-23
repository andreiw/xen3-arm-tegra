########################################
# arm-specific definitions

#
# If you change any of these configuration options then you must
# 'make clean' before rebuilding.
#

supervisor_mode_kernel ?= n

CFLAGS  += -nostdinc -fno-builtin -fno-common -fno-strict-aliasing -mno-thumb-interwork
CFLAGS  += -iwithprefix include -Wno-pointer-arith -pipe
CFLAGS  += -I$(BASEDIR)/include -I$(BASEDIR)/include/security -I$(BASEDIR)/include/security/crypto

ifeq ($(CONFIG_EABI_SUPPORT),y)
CFLAGS +=-mabi=aapcs-linux -mno-thumb-interwork
endif

machine-$(CONFIG_MACHINE_IMX21)       := imx21

TARGET_MACHINE := $(machine-y)

# Test for at least GCC v3.2.x.
gcc-ver = $(shell $(CC) -dumpversion | sed -e 's/^\(.\)\.\(.\)\.\(.\)/\$(1)/')

ifeq ($(call gcc-ver,1),1)
$(error gcc-1.x.x unsupported - upgrade to at least gcc-3.2.x)
endif
ifeq ($(call gcc-ver,1),2)
$(error gcc-2.x.x unsupported - upgrade to at least gcc-3.2.x)
endif
ifeq ($(call gcc-ver,1),3)
ifeq ($(call gcc-ver,2),0)
$(error gcc-3.0.x unsupported - upgrade to at least gcc-3.2.x)
endif
ifeq ($(call gcc-ver,2),1)
$(error gcc-3.1.x unsupported - upgrade to at least gcc-3.2.x)
endif
endif



