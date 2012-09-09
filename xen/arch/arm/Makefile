#
# xen/arch/arm/Makefile
#

include $(BASEDIR)/Rules.mk

ifndef TARGET_MACHINE
$(error Unknown target platform)
endif

subdir-y += arch-$(TARGET_MACHINE) xen lib 

include $(BASEDIR)/Post.mk

OBJCOPYFLAGS    :=-O binary -R .note -R .comment -S

$(TARGET): $(TARGET)-bin $(TARGET)-syms
	@cp $(TARGET)-syms $@

$(TARGET)-syms: arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) xen.lds
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) -o $@

$(TARGET)-bin: $(TARGET)-syms
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

asm-offsets.s: xen/asm-offsets.c $(HDRS)
	$(CC) $(CFLAGS) -S -o $@ $<

xen.lds: xen/xen.lds.S 
	$(CC) -E $(CFLAGS) -P -DXEN -D_XEN_ -D__XEN__ $(AFLAGS) \
	      -o $@ $<

clean:: FORCE
	rm -f arch/arm/*.o arch/arm/arch-$(TARGET_MACHINE)/*.o lib/*.o xen/*.o xen.lds
