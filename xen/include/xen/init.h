#ifndef XEN_INIT_H
#define XEN_INIT_H

#include <xen/config.h>
#include <asm/init.h>

/* These macros are used to mark some functions or
 * initialized data (doesn't apply to uninitialized data)
 * as `initialization' functions. The kernel can take this
 * as hint that the function is used only during the initialization
 * phase and free up used memory resources after
 *
 * Usage:
 * For functions:
 *
 * You should add __init immediately before the function name, like:
 *
 * static void __init initme(int x, int y)
 * {
 *    extern int z; z = x * y;
 * }
 *
 * If the function has a prototype somewhere, you can also add
 * __init between closing brace of the prototype and semicolon:
 *
 * extern int initialize_foobar_device(int, int, int) __init;
 *
 * For initialized data:
 * You should insert __initdata between the variable name and equal
 * sign followed by value, e.g.:
 *
 * static int init_variable __initdata = 0;
 * static char xen_logo[] __initdata = { 0x32, 0x36, ... };
 *
 * Don't forget to initialize data not at file scope, i.e. within a function,
 * as gcc otherwise puts the data into the bss section and not into the init
 * section.
 *
 * Also note, that this data cannot be "const".
 */

#ifndef __ASSEMBLY__

/*
 * Used for initialization calls..
 */
typedef int (*initcall_t)(void);

extern initcall_t __initcall_start, __initcall_end;

#define __initcall(fn) \
    static initcall_t __initcall_##fn __init_call = fn

/*
 * Used for kernel command line parameter setup
 */
struct kernel_param {
    const char *name;
    enum { OPT_STR, OPT_UINT, OPT_BOOL, OPT_CUSTOM } type;
    void *var;
    unsigned int len;
};

extern struct kernel_param __setup_start, __setup_end;

#define custom_param(_name, _var) \
    static char __setup_str_##_var[] __initdata = _name; \
    static struct kernel_param __setup_##_var __attribute_used__ \
        __initsetup = { __setup_str_##_var, OPT_CUSTOM, &_var, 0 }
#define boolean_param(_name, _var) \
    static char __setup_str_##_var[] __initdata = _name; \
    static struct kernel_param __setup_##_var __attribute_used__ \
        __initsetup = { __setup_str_##_var, OPT_BOOL, &_var, sizeof(_var) }
#define integer_param(_name, _var) \
    static char __setup_str_##_var[] __initdata = _name; \
    static struct kernel_param __setup_##_var __attribute_used__ \
        __initsetup = { __setup_str_##_var, OPT_UINT, &_var, sizeof(_var) }
#define string_param(_name, _var) \
    static char __setup_str_##_var[] __initdata = _name; \
    static struct kernel_param __setup_##_var __attribute_used__ \
        __initsetup = { __setup_str_##_var, OPT_STR, &_var, sizeof(_var) }

#endif /* __ASSEMBLY__ */

#endif /* XEN_INIT_H */

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
