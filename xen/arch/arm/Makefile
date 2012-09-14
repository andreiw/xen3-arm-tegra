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

$(TARGET): $(TARGET)-bin $(TARGET)-pass3
	@cp $(TARGET)-pass3 $@

$(TARGET)-syms: arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) xen.lds
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) -o $@

$(TARGET)-pass1.S: $(TARGET)-syms
	$(NM) $< | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > $@.map
	$(BASEDIR)/tools/symbols < $@.map > $@

$(TARGET)-pass1: $(TARGET)-pass1.o arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) xen.lds
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds $(TARGET)-pass1.o arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) -o $@

$(TARGET)-pass2.S: $(TARGET)-pass1
	$(NM) $< | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > $@.map
	$(BASEDIR)/tools/symbols --all-symbols < $@.map > $@

$(TARGET)-pass2: $(TARGET)-pass2.o arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) xen.lds
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds $(TARGET)-pass2.o arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) -o $@

$(TARGET)-pass3.S: $(TARGET)-pass2
	$(NM) $< | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > $@.map
	$(BASEDIR)/tools/symbols --all-symbols < $@.map > $@

$(TARGET)-pass3: $(TARGET)-pass3.o arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) xen.lds
	$(LD) $(LDFLAGS) -Map xen.map -N -T xen.lds $(TARGET)-pass3.o arch-$(TARGET_MACHINE)/start.o $(ALL_OBJS) -o $@

$(TARGET)-bin: $(TARGET)-pass3
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

asm-offsets.s: xen/asm-offsets.c $(HDRS)
	$(CC) $(CFLAGS) -S -o $@ $<

xen.lds: xen/xen.lds.S 
	$(CC) -E $(CFLAGS) -P -DXEN -D_XEN_ -D__XEN__ $(AFLAGS) \
	      -o $@ $<

clean:: FORCE
	rm -f arch/arm/*.o arch/arm/arch-$(TARGET_MACHINE)/*.o lib/*.o xen/*.o xen.lds
