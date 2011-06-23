
#ifndef __MYGEN_H__

typedef struct __mygen_result {
	int cpu0;
	int cpu1;
	int mem0;
	int mem1;
} mygen_result;

int mygeninit();
void mygenclose();
int mygen(mygen_result *);
int mygen_numdomains();

#endif
