#ifndef __LIB_H__
#define __LIB_H__

#include <xen/inttypes.h>
#include <stdarg.h>
#include <xen/config.h>
#include <xen/types.h>
#include <xen/xmalloc.h>
#include <xen/string.h>

#define BUG()								\
	do {								\
		printk("BUG at %s:%d\n", __FILE__, __LINE__);		\
		panic("BUG\n");						\
		while(1);						\
	} while ( 0 )

#define BUG_ON(_p) do { if (_p) BUG(); } while ( 0 )

#ifndef NDEBUG
#define ASSERT(_p) { if ( !(_p) ) { printk("Assertion '%s' failed, line %d, file %s\n", #_p , __LINE__, __FILE__); BUG(); } }
#else
#define ASSERT(_p) ((void)0)
#endif

#define SWAP(_a, _b) \
   do { typeof(_a) _t = (_a); (_a) = (_b); (_b) = _t; } while ( 0 )

#define DIV_ROUND(x, y) (((x) + (y) / 2) / (y))

#define reserve_bootmem(_p,_l) ((void)0)

struct domain;

void cmdline_parse(char *cmdline);

#ifndef NDEBUG
extern int debugtrace_send_to_console;
extern void debugtrace_dump(void);
extern void debugtrace_printk(const char *fmt, ...);
#else
#define debugtrace_dump()          ((void)0)
#define debugtrace_printk(_f, ...) ((void)0)
#endif

void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
                        int groupsize, char *linebuf, size_t linebuflen,
                        bool ascii);

/* Allows us to use '%p' as general-purpose machine-word format char. */
#define _p(_x) ((void *)(unsigned long)(_x))
#define printk(_f , _a...) printf( _f , ## _a )
extern void printf(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
extern void panic(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
extern long vm_assist(struct domain *, unsigned int, unsigned int);

/* vsprintf.c */
extern int sprintf(char * buf, const char * fmt, ...)
    __attribute__ ((format (printf, 2, 3)));
extern int vsprintf(char *buf, const char *, va_list)
    __attribute__ ((format (printf, 2, 0)));
extern int snprintf(char * buf, size_t size, const char * fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
    __attribute__ ((format (printf, 3, 0)));
extern int scnprintf(char * buf, size_t size, const char * fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
    __attribute__ ((format (printf, 3, 0)));

int vsscanf(const char *buf, const char *fmt, va_list args);
int sscanf(const char *buf, const char *fmt, ...);

long simple_strtol(
    const char *cp,char **endp, unsigned int base);
unsigned long simple_strtoul(
    const char *cp,char **endp, unsigned int base);
long long simple_strtoll(
    const char *cp,char **endp, unsigned int base);
unsigned long long simple_strtoull(
    const char *cp,char **endp, unsigned int base);

unsigned long long parse_size_and_unit(char *s);

#define TAINT_UNSAFE_SMP                (1<<0)
#define TAINT_MACHINE_CHECK             (1<<1)
#define TAINT_BAD_PAGE                  (1<<2)
extern int tainted;
#define TAINT_STRING_MAX_LEN            20
extern char *print_tainted(char *str);
extern void add_taint(unsigned);

#endif /* __LIB_H__ */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
