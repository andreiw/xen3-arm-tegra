#ifndef XEN_ARM_INIT_H
#define XEN_ARM_INIT_H

#ifndef __ASSEMBLY__
/*
 * Mark functions and data as being only used at initialization
 * or exit time.
 */
#define __init       \
    __attribute__ ((__section__ (".init.text")))

#define __initdata   \
    __attribute__ ((__section__ (".init.data")))
#define __initsetup  \
    __attribute_used__ __attribute__ ((__section__ (".init.setup")))
#define __init_call  \
    __attribute_used__ __attribute__ ((__section__ (".initcall.init")))
#endif

#endif /* XEN_ARM_INIT_H */
