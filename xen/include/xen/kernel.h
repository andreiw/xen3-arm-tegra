#ifndef XEN_KERNEL_H
#define XEN_KERNEL_H

#include <xen/types.h>
#include <stdarg.h>
#include <xen/errno.h>
#include <xen/string.h>
#include <xen/xmalloc.h>
#include <xen/init.h>
#include <xen/ctype.h>

/*
 * 'kernel.h' contains some often-used function prototypes, and
 * things that don't belong elsewhere.
 */

#define BUG() do {                                 \
      panic("BUG at %s:%d\n", __FILE__, __LINE__); \
   } while (0)

#define BUG_ON(what) do {                                            \
      if ((what)) {                                                  \
         panic("BUG_ON(" #what ") at %s:%d\n", __FILE__, __LINE__);  \
      }                                                              \
   } while (0)

#ifndef NDEBUG
#define ASSERT(what) do {                                            \
      if (!(what)) {                                                 \
         panic("ASSERT(" #what ") at %s:%d\n", __FILE__, __LINE__);  \
      }                                                              \
   } while (0)
#else
#define ASSERT(what)
#endif

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x,y) ({                             \
         const typeof(x) _x = (x);              \
         const typeof(y) _y = (y);              \
         (void) (&_x == &_y);                   \
         _x < _y ? _x : _y; })

#define max(x,y) ({                             \
         const typeof(x) _x = (x);              \
         const typeof(y) _y = (y);              \
         (void) (&_x == &_y);                   \
         _x > _y ? _x : _y; })

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max at all, of course.
 */
#define min_t(type,x,y)                                        \
   ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y)                                        \
   ({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr: the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                     \
         const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
         (type *)( (char *)__mptr - offsetof(type,member) );})

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x)                       \
   ({ type __dummy;                             \
      typeof(x) __dummy2;                       \
      (void)(&__dummy == &__dummy2);            \
      1;                                        \
   })

#define ROUND_UP(what, by)   (((what) + (by) - 1) & ~((by) - 1))
#define ROUND_DOWN(what, by) ((what) & ~((by) - 1))

void cmdline_parse(char *cmdline);

#ifndef NDEBUG
extern int debugtrace_send_to_console;
extern void debugtrace_dump(void);
extern void debugtrace_printk(const char *fmt, ...);
#else
#define debugtrace_dump()
#define debugtrace_printk(_f, ...)
#endif

void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
                        int groupsize, char *linebuf, size_t linebuflen,
                        bool ascii);

#define printk(_f , _a...) printf( _f , ## _a )
void printf(const char *format, ...);
void panic(const char *format, ...);

/* vsprintf.c */
int sprintf(char * buf, const char * fmt, ...);
int vsprintf(char *buf, const char *, va_list);
int snprintf(char * buf, size_t size, const char * fmt, ...);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int scnprintf(char * buf, size_t size, const char * fmt, ...);
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

int vsscanf(const char *buf, const char *fmt, va_list args);
int sscanf(const char *buf, const char *fmt, ...);
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);
long simple_strtol(const char *cp, char **endp, unsigned int base);
unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);
long long simple_strtoll(const char *cp,char **endp,unsigned int base);

#endif /* XEN_KERNEL_H */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
