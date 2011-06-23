/******************************************************************************
 * sysctl.h
 * 
 * System management operations. For use by node control stack.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Copyright (c) 2002-2006, K Fraser
 */

#ifndef __XEN_PUBLIC_SYSCTL_H__
#define __XEN_PUBLIC_SYSCTL_H__

/* FIXME: by SRC - the lines below are present in original sysctl.h, we leave them commented for some time, but soon get rid of'em */
/*
#if !defined(__XEN__) && !defined(__XEN_TOOLS__)
#error "sysctl operations are intended for use by node control tools only"
#endif
*/

#include "xen.h"

#define XEN_SYSCTL_INTERFACE_VERSION 0x00000003 /* FIXME: by SRC */

#define uint64_aligned_t uint64_t /* FIXME: by SRC - already defined in xen.h under __XEN__ && __XEN_TOOLS__, see fixme comment above */

#define XEN_SYSCTL_ltt_op           8
struct xen_sysctl_ltt_op {
	/* IN variables */
#define XEN_SYSCTL_LTTOP_get_info     0
#define XEN_SYSCTL_LTTOP_get_cpu      1
#define XEN_SYSCTL_LTTOP_put_cpu      2
#define XEN_SYSCTL_LTTOP_get_facilities 3
#define XEN_SYSCTL_LTTOP_put_facilities 4
#define XEN_SYSCTL_LTTOP_create       5
#define XEN_SYSCTL_LTTOP_destroy      6
#define XEN_SYSCTL_LTTOP_start        7
#define XEN_SYSCTL_LTTOP_stop         8
	uint32_t cmd;
	uint32_t mode;	/* 0: normal, 1: flight, 2: hybrid */
	uint32_t cpu;
	/* IN/OUT variables */
	uint32_t cpu_subbuf_size;
	uint32_t cpu_n_subbufs;
	uint32_t facilities_subbuf_size;
	uint32_t facilities_n_subbufs;
	uint32_t consumer_offset;
	uint32_t num_cpus;
	/* OUT variables */
	uint64_aligned_t cpu_buffer_mfn;
	uint64_aligned_t facilities_buffer_mfn;
};
typedef struct xen_sysctl_ltt_op xen_sysctl_ltt_op_t;
DEFINE_GUEST_HANDLE(xen_sysctl_ltt_op_t);

struct xen_sysctl {
	uint32_t cmd;
	uint32_t interface_version; /* XEN_SYSCTL_INTERFACE_VERSION */
	union {
		struct xen_sysctl_ltt_op            ltt_op;
		uint8_t                             pad[128];
	} u;
};

typedef struct xen_sysctl xen_sysctl_t;
DEFINE_GUEST_HANDLE(xen_sysctl_t);

#endif /* __XEN_PUBLIC_SYSCTL_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
