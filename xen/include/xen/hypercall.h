/******************************************************************************
 * hypercall.h
 */

#ifndef __XEN_HYPERCALL_H__
#define __XEN_HYPERCALL_H__

#include <xen/config.h>
#include <xen/time.h>
#include <public/xen.h>
#include <public/event_channel.h>
#include <asm/hypercall.h>

extern long
do_ni_hypercall(
    void);

extern long
do_sched_op_compat(
    int cmd,
    unsigned long arg);

extern long
do_sched_op(
    int cmd,
    GUEST_HANDLE(void) arg);

extern long
do_dom0_op(
    GUEST_HANDLE(dom0_op_t) u_dom0_op);

extern long
do_memory_op(
    int cmd,
    GUEST_HANDLE(void) arg);

extern long
do_multicall(
    GUEST_HANDLE(multicall_entry_t) call_list,
    unsigned int nr_calls);

extern long
do_set_timer_op(
    s_time_t timeout);

extern long
do_get_system_time(void);

extern long
do_event_channel_op(
    GUEST_HANDLE(evtchn_op_t) uop);

extern long
do_xen_version(
    int cmd,
    GUEST_HANDLE(void) arg);

extern long
do_console_io(
    int cmd,
    int count,
    GUEST_HANDLE(char) buffer);

extern long
do_grant_table_op(
    unsigned int cmd,
    GUEST_HANDLE(void) uop,
    unsigned int count);

extern long
do_vm_assist(
    unsigned int cmd,
    unsigned int type);

extern long
do_vcpu_op(
    int cmd,
    int vcpuid,
    GUEST_HANDLE(void) arg);

extern long
do_nmi_op(
    unsigned int cmd,
    GUEST_HANDLE(void) arg);

#endif /* __XEN_HYPERCALL_H__ */
