#include <xen/kernel.h>
#include <xen/lib.h>
#include <xen/debugger.h>
#include <asm/page.h>
#include <asm/tlb.h>

#define V2P_PAR_FAULT              (1 << 0)
#define V2P_PAR_FAULT_STATUS_SHIFT (1)
#define V2P_PAR_FAULT_STATUS_MASK  (0x1F)

#define V2P_PAR_SUPERSECTION       (1 << 1)
#define V2P_PAR_OUTER_SHIFT        (2)
#define V2P_PAR_OUTER_MASK         (0x3)
#define V2P_PAR_OUTER_WBNWA        (3)
#define V2P_PAR_OUTER_WTNWA        (2)
#define V2P_PAR_OUTER_WBWA         (1)
#define V2P_PAR_OUTER_NC           (0)
#define V2P_PAR_INNER_SHIFT        (4)
#define V2P_PAR_INNER_MASK         (7)
#define V2P_PAR_INNER_WBNWA        (7)
#define V2P_PAR_INNER_WT           (6)
#define V2P_PAR_INNER_WBWA         (5)
#define V2P_PAR_INNER_DEVICE       (3)
#define V2P_PAR_INNER_SO           (1)
#define V2P_PAR_INNER_NC           (0)
#define V2P_PAR_SHAREABLE          (1 << 7)
#define V2P_PAR_IMPL               (1 << 8)
#define V2P_PAR_NON_SECURE         (1 << 9)
#define V2P_PAR_OUTER_SHAREABLE    (1 << 10)
#define V2P_PAR_PA_SHIFT           (12)
#define V2P_PAR_FLAGS              (V2P_PAR_SUPERSECTION |  \
                                    V2P_PAR_SHAREABLE |     \
                                    V2P_PAR_IMPL |          \
                                    V2P_PAR_NON_SECURE)

static u32 tlb_v2p_check(vaddr_t va, bool priviledged, bool write)
{
   u32 par;

   if (priviledged) {
      if (write) {
         asm volatile("mcr p15, 0, %0, c7, c8, 1" : : "r"(va) : "cc");
      } else {
         asm volatile("mcr p15, 0, %0, c7, c8, 0" : : "r"(va) : "cc");
      }
   } else {
      if (write) {
         asm volatile("mcr p15, 0, %0, c7, c8, 3" : : "r"(va) : "cc");
      } else {
         asm volatile("mcr p15, 0, %0, c7, c8, 2" : : "r"(va) : "cc");
      }
   }

   isb();
   asm volatile("mrc p15, 0, %0, c7, c4, 0" : "=r"(par) : : "cc");
   return par;
}

bool tlb_check(vaddr_t va, vaddr_t end, bool priviledged, bool write)
{
   va = ROUND_DOWN(va, PAGE_SIZE);
   end = ROUND_DOWN(end, PAGE_SIZE);

   while (va <= end) {
      if (tlb_v2p_check(va, priviledged, write) & V2P_PAR_FAULT) {
         return false;
      }
      va += PAGE_SIZE;
   }

   return true;
}

static void debug_tlb_v2p(struct debug_command *cmd,
                          const char *arg,
                          debug_printf_cb cb)
{
   char p;
   char a;
   u32 par;
   u8 flags;
   vaddr_t va;

   if ((sscanf(arg, "%c %c 0x%x", &p, &a, &va) != 3) ||
       (p != 'u' && p != 'p') ||
       (a != 'r' && a != 'w')) {
      cb("Usage: %s u|p r|w <hex VA address>\n", cmd->name);
      return;
   }

   cb("%s %s VA 0x%08x to PA: ", p == 'u' ?
      "Unpriviledged" : "Priviledged", a == 'r' ?
      "read" : "write", va);

   par = tlb_v2p_check(va, p == 'p', a == 'w');

   /* Now let's parse the PAR. */
   if (par & V2P_PAR_FAULT) {
      cb("fault, status = 0x%x\n",
         (par >> V2P_PAR_FAULT_STATUS_SHIFT) &
         V2P_PAR_FAULT_STATUS_MASK);
      return;
   }

   /* Not a fault. */
   cb("0x%08x\nPAR: 0x08%x ", (par >> V2P_PAR_PA_SHIFT) <<
      V2P_PAR_PA_SHIFT, par);
   if (par & V2P_PAR_FLAGS) {
      cb("flags: ");
      if (par & V2P_PAR_SUPERSECTION) {
         cb("super-section ");
      }
      if (par & V2P_PAR_SHAREABLE) {
         cb("shareable ");
      }
      if (par & V2P_PAR_IMPL) {
         cb("implementation-specific ");
      }
      if (par & V2P_PAR_OUTER_SHAREABLE) {
         cb("outer-shareable ");
      }
   }
   cb("\nOuter: ");
   flags = (par >> V2P_PAR_OUTER_SHIFT) & V2P_PAR_OUTER_MASK;
   switch (flags) {
   case V2P_PAR_OUTER_WBNWA:
      cb("write-back, no write-allocate\n");
      break;
   case V2P_PAR_OUTER_WTNWA:
      cb("write-through, no write-allocate\n");
      break;
   case V2P_PAR_OUTER_WBWA:
      cb("write-back, write-allocate\n");
      break;
   case V2P_PAR_OUTER_NC:
      cb("non-cacheable\n");
      break;
   default:
      cb("unknown 0x%x\n", flags);
   }
   cb("Inner: ");
   flags = (par >> V2P_PAR_INNER_SHIFT) & V2P_PAR_INNER_MASK;
   switch (flags) {
   case V2P_PAR_INNER_WBNWA:
      cb("write-back, no write-allocate\n");
      break;
   case V2P_PAR_INNER_WT:
      cb("write-through\n");
      break;
   case V2P_PAR_INNER_WBWA:
      cb("write-back, write-allocate\n");
      break;
   case V2P_PAR_INNER_DEVICE:
      cb("device\n");
      break;
   case V2P_PAR_INNER_SO:
      cb("strongly ordered\n");
      break;
   case V2P_PAR_INNER_NC:
      cb("non-cached\n");
      break;
   default:
      cb("unknown 0x%x\n", flags);
   }
}

DEBUG_COMMAND(v2p, debug_tlb_v2p,
              "VA to PA translation");

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
