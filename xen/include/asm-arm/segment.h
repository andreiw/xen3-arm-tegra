#ifndef __ASM_SEGMENT_H__
#define __ASM_SEGMENT_H__

#define ST_HYPERVISOR	0
#define ST_BOOTPAGE	1

#ifndef __ASSEMBLY__
struct segment {
	unsigned long 	base;
	unsigned long	size;
	unsigned long	type;
};
#endif

void register_segment(struct segment *seg_base, int entries);
void unregister_segment(struct segment *seg_base, int entries);

#endif

