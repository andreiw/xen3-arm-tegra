#ifndef __XEN_COMPILER_H__
#define __XEN_COMPILER_H__

#if __GNUC__ < 4
#error Unsupported GCC :-(.
#endif

#define __deprecated        __attribute__((deprecated))
#define __packed            __attribute__((packed))
#define __weak              __attribute__((weak))
#define __naked             __attribute__((naked)) noinline __noclone
#define __noreturn          __attribute__((noreturn))
#define __pure              __attribute__((pure))
#define __aligned(x)        __attribute__((aligned(x)))
#define __printf(a,b)       __attribute__((format(printf,a,b)))
#define  noinline           __attribute__((noinline))
#define __attribute_const__ __attribute__((__const__))
#define __attribute_used__  __attribute__((__used__))
#define __maybe_unused      __attribute__((unused))
#define __always_unused     __attribute__((unused))

#if __GNUC_MINOR__ >= 5
#define __noclone           __attribute__((__noclone__))
#else
#define __noclone
#endif

#define likely(x)           __builtin_expect((x),1)
#define unlikely(x)         __builtin_expect((x),0)

#endif /* __XEN_COMPILER_H__ */
