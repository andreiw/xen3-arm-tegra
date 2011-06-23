#ifndef __ARM_BUG_H__
#define __ARM_BUG_H__

#define BUGFRAME_dump   0
#define BUGFRAME_warn   1
#define BUGFRAME_bug    2
#define BUGFRAME_assert 3

struct bug_frame {
	unsigned char ud2[2];
	unsigned char ret;
	unsigned short id; /* BUGFRAME_??? */
};

#endif

