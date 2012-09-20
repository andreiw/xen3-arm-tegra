#ifndef __ASM_STRING_H__
#define __ASM_STRING_H__

/*
 * We don't do inline string functions, since the
 * optimised inline asm versions are not small.
 */

#define __HAVE_ARCH_STRRCHR
char * strrchr(const char * s, int c);

#define __HAVE_ARCH_STRCHR
char * strchr(const char * s, int c);

#define __HAVE_ARCH_MEMCPY
void * memcpy(void *, const void *, size_t);

#define __HAVE_ARCH_MEMMOVE
void * memmove(void *, const void *, size_t);

#define __HAVE_ARCH_MEMCHR
void * memchr(const void *, int, size_t);

#define __HAVE_ARCH_MEMZERO
#define __HAVE_ARCH_MEMSET
void * memset(void *, int, size_t);

#define __HAVE_ARCH_BCOPY

void __memzero(void *ptr, size_t n);

#define memset(p,v,n)							\
	({								\
		if ((n) != 0) {						\
			if (__builtin_constant_p((v)) && (v) == 0)	\
				__memzero((p),(n));			\
			else						\
				memset((p),(v),(n));			\
		}							\
		(p);							\
	})

#define memzero(p,n) ({ if ((n) != 0) __memzero((p),(n)); (p); })

#endif
