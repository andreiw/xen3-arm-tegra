#ifndef XEN_STRING_H_
#define XEN_STRING_H_

char * strpbrk(const char *,const char *);
char * strsep(char **,const char *);
size_t strspn(const char *,const char *);

/*
 * Include machine specific inline routines
 */
#include <asm/string.h>

#ifndef __HAVE_ARCH_STRCPY
char * strcpy(char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRNCPY
char * strncpy(char *,const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRLCPY
size_t strlcpy(char *,const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCAT
char * strcat(char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCAT
char * strncat(char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *,const char *,size_t);
#endif
#ifndef __HAVE_ARCH_STRNICMP
int strnicmp(const char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCHR
char * strchr(const char *,int);
#endif
#ifndef __HAVE_ARCH_STRRCHR
char * strrchr(const char *,int);
#endif
#ifndef __HAVE_ARCH_STRSTR
char * strstr(const char *,const char *);
#endif
#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char *);
#endif
#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *,size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
void * memset(void *,int,size_t);
#endif
#ifndef __HAVE_ARCH_MEMCPY
void * memcpy(void *,const void *,size_t);
#endif
#ifndef __HAVE_ARCH_MEMMOVE
void * memmove(void *,const void *,size_t);
#endif
#ifndef __HAVE_ARCH_MEMSCAN
void * memscan(void *,int,size_t);
#endif
#ifndef __HAVE_ARCH_MEMCMP
int memcmp(const void *,const void *,size_t);
#endif
#ifndef __HAVE_ARCH_MEMCHR
void * memchr(const void *,int,size_t);
#endif

#define safe_strcpy(d,s)                        \
  do { strncpy((d),(s),sizeof((d)));            \
    (d)[sizeof((d))-1] = '\0';                  \
  } while (0)

#endif /* XEN_STRING_H_ */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
