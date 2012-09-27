include $(BASEDIR)/Rules.mk

obj-y += console.o
obj-y += serial.o
obj-y += fbcon.o
obj-y += font_sun12x22.o

include $(BASEDIR)/Post.mk

# Object file contains changeset and compiler information.
console.o: $(BASEDIR)/include/xen/compile.h
