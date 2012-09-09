/******************************************************************************
 * iocap.h
 * 
 * Per-domain I/O capabilities.
 */

#ifndef __XEN_IOCAP_H__
#define __XEN_IOCAP_H__

#include <xen/rangeset.h>

#define ioports_permit_access(d, s, e)                  \
    rangeset_add_range((d)->arch.ioport_caps, s, e)
#define ioports_deny_access(d, s, e)                    \
    rangeset_remove_range((d)->arch.ioport_caps, s, e)
#define ioports_access_permitted(d, s, e)               \
    rangeset_contains_range((d)->arch.ioport_caps, s, e)

#define cache_flush_permitted(d)                \
  (!rangeset_is_empty((d)->iomem_caps))

#define iomem_permit_access(d, s, e)                    \
  rangeset_add_range((d)->iomem_caps, s, e)
#define iomem_access_permitted(d, s, e)                 \
  rangeset_contains_range((d)->iomem_caps, s, e)

#define iomem_deny_access(d, s, e)                      \
    rangeset_remove_range((d)->iomem_caps, s, e)

#define irq_permit_access(d, i)                         \
  rangeset_add_singleton((d)->irq_caps, i)
#define irqs_permit_access(d, s, e)                     \
  rangeset_add_range((d)->irq_caps, s, e)
#define irq_access_permitted(d, i)                      \
  rangeset_contains_singleton((d)->irq_caps, i)

#define irq_deny_access(d, i)                           \
    rangeset_remove_singleton((d)->irq_caps, i)
#define irqs_deny_access(d, s, e)                       \
    rangeset_remove_range((d)->irq_caps, s, e)

#define multipage_allocation_permitted(d)       \
  (!rangeset_is_empty((d)->iomem_caps))

#define dmachn_permit_access(d, i)                         \
    rangeset_add_singleton((d)->dma_caps, i)
#define dmachn_deny_access(d, i)                           \
    rangeset_remove_singleton((d)->dma_caps, i)
#define dmachns_permit_access(d, s, e)                     \
    rangeset_add_range((d)->dma_caps, s, e)
#define dmachns_deny_access(d, s, e)                       \
    rangeset_remove_range((d)->dma_caps, s, e)
#define dmachn_access_permitted(d, i)                      \
    rangeset_contains_singleton((d)->dma_caps, i)

#endif /* __XEN_IOCAP_H__ */
