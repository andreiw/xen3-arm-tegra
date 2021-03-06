/*
 * start.S
 *
 * Copyright (C) 2008 Samsung Electronics
 * Copyrigjt (C) 2008 ChanJu Park  <bestworld@samsung.com>
 * Copyright (C) 2011 Andrei Warkentin <andreiw@msalumni.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <xen/linkage.h>
#include <xen/config.h>
#include <asm/cpu-domain.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/arch/hardware.h>
#include <asm/pt.h>

        .type   text, %function
ENTRY(text)
        .rept   8
        mov     r0, r0
        .endr

        b       1f
        .word   0x016f2818              @ Magic numbers to help the loader
        .word   text
        .word   _end
        
        @ Save ATAG and dump some info.
1:      adr     r1, __ATAG_INFO
        str     r2, [r1, #0]
        mov     ip, r2
        adr     r0, str_p0
        bl      printascii
        mov     r0, ip
        bl      printhex8
        adr     r0, str_p1
        bl      printascii
        adr     r0, LC0
        ldmia   r0, {r1, r2, r3, r4}
        mov     r0, r2
        bl      printhex8
        adr     r0, str_p2
        bl      printascii
        adr     r0, text
        bl      printhex8
        adr     r0, str_p3
        bl      printascii
        ldr     r0, =(KERNEL_VIRT_LINKED - KERNEL_VIRT_BASE + KERNEL_PHYS_BASE)
        bl      printhex8
        adr     r0, str_n
        bl      printascii
        b       1f
str_p0: .asciz  "\r\nATAG    (P): 0x"
str_p1: .asciz  "\r\nLinked  (V): 0x"
str_p2: .asciz  "\r\nActual  (P): 0x"
str_p3: .asciz  "\r\nDesired (P): 0x"
str_n:  .asciz  "\r\n"
        .align
1:      msr     cpsr_c, #PSR_F_BIT | PSR_I_BIT | PSR_MODE_SVC

        @ Enable Strict alignment checking & Instruction cache.
        mrc     p15, 0, r0, c1, c0, 0
        bic     r0, r0, #0x00002000     @ clear bits 13 (V)
        bic     r0, r0, #0x00000005     @ clear bits 0 (M), 2 (C)
        orr     r0, r0, #0x00000002     @ set bit 1 (A) Align
        orr     r0, r0, #0x00001000     @ set bit 12 (I) enable I-Cache
        orr     r0, r0, #0x00020000     @ set bit 17 (HA)
        orr     r0, r0, #0x20000000     @ set bit 29 (AFE) access flag
        mcr     p15, 0, r0, c1, c0, 0
        mcr     p15, 0, r10, c8, c7, 0  @ invalidate I + D TLBs
        
        @ Relocate to kernel desired physical address.
        adr     r0, LC0
        ldmia   r0, {r1, r2, r3, r4}            @ r2 - linked _start
        sub     r1, r1, #KERNEL_VIRT_BASE
        add     r1, r1, #KERNEL_PHYS_BASE
        sub     r2, r2, #KERNEL_VIRT_BASE
        add     r2, r2, #KERNEL_PHYS_BASE
        sub     r3, r3, #KERNEL_VIRT_BASE
        add     r3, r3, #KERNEL_PHYS_BASE
        sub     r4, r4, #KERNEL_VIRT_BASE
        add     r4, r4, #KERNEL_PHYS_BASE
        cmp     r0, r1
        beq     3f      @ source == destination
        bhs     2f      @ source > destination

        @ source < destination
        sub     r0, r1, r0                      @ r0 = desired - actual
        sub     r0, r3, r0                      @ r0 = actual _end
        sub     r0, r0, #4
        sub     r3, r3, #4
1:      ldr     r1, [r0], #-4
        str     r1, [r3], #-4
        cmp     r3, r2
        blo     1b
        b       3f

        @ source > destination
2:      sub     r0, r1, r0                      @ r0 = desired - actual
        sub     r0, r2, r0                      @ r0 = actual _start
2:      ldr     r1, [r0], #4
        str     r1, [r2], #4
        cmp     r2, r3
        blo     2b
3:      isb
        dsb
        mov     pc, r4
reloc_done:
        

        @ PGD is set up 16K below the load address.
pgd_setup:
        adr     r0, text
        mov     r1, r0
        sub     r0, r0, #0x4000
        mov     r2, #0
1:      str     r2, [r1, #-4]!
        str     r2, [r1, #-4]!
        str     r2, [r1, #-4]!
        str     r2, [r1, #-4]!
        cmp     r0, r1
        bne     1b

        adr     r4, mem_map_table
        b       2f

1:      str     r3, [r0, r2]
        add     r2, r2, #4
        add     r3, r3, #0x100000
        adds    r1, r1, #-1
        bhi     1b

2:      ldmia   r4!, {r1, r2, r3}
        cmp     r1, #0
        bne     1b

dac_setup:
        mov     r5, #(DOMAIN_VALUE(DOMAIN_SUPERVISOR, DOMAIN_CLIENT)   | \
                DOMAIN_VALUE(DOMAIN_HYPERVISOR, DOMAIN_CLIENT)         | \
                DOMAIN_VALUE(DOMAIN_USER, DOMAIN_CLIENT)               | \
                DOMAIN_VALUE(DOMAIN_IO, DOMAIN_CLIENT))

        mcr     p15, 0, r5, c3, c0, 0   @ Load DAC
        mcr     p15, 0, r0, c2, c0, 0   @ Load page table pointer
        mrc     p15, 0, r6, c1, c0, 0

        orr     r6, r6, #CR_M           @ Enable Memory Management Unit
        orr     r6, r6, #CR_C           @ Enable Data Cache
        orr     r6, r6, #CR_W           @ Enable Write Buffer
        orr     r6, r6, #CR_I           @ Enable Instruction Cache

        mcr     p15, 0, r6, c1, c0, 0   @ Turn on MMU
        mrc     p15, 0, r3, c0, c0, 0
        mov     r0, r0
        mov     r0, r0
        mov     r0, r0

        mcr     p15, 0, r10, c8, c7, 0  @ invalidate I + D TLBs
        b       clear_bss

1:      .word   _sbss
        .word   _ebss

clear_bss:
        adr     r0, 1b
        ldmia   r0, {r1, r2}
        mov     r0, #0
1:
        str     r0, [r1], #4
        cmp     r1, r2
        blo     1b

stack_setup:
        msr     cpsr_c, #PSR_MODE_IRQ | PSR_I_BIT | PSR_F_BIT
        ldr     sp, =(irq_stack + STACK_SIZE)

        msr     cpsr_c, #PSR_MODE_ABT | PSR_I_BIT | PSR_F_BIT
        ldr     sp, =(abt_stack + STACK_SIZE)

        msr     cpsr_c, #PSR_MODE_UND | PSR_I_BIT | PSR_F_BIT
        ldr     sp, =(und_stack + STACK_SIZE)

        msr     cpsr_c, #PSR_MODE_SVC | PSR_I_BIT | PSR_F_BIT
        ldr     sp, =(svc_stack + STACK_SIZE)

        adr     r0, boot_vectors_loc
        ldr     r0, [r0]
        mcr     p15, 0, r0, c12, c0, 0

        adr     r0, str_h
        bl      printascii
        b       1f
str_h:  .asciz  "\r\nHanding off to C...\r\n"
        .align
1:      adr     r12, 1f
        ldmia   r12, {lr, pc}
1:      .long   0
        .long   start_xen

ENTRY(arch_context_switch)
        mov     pc, lr

mem_map_table:

        @ XXX: Identity mapped 1GB, temporary.
        MAP_ENTRY(TEGRA_RAM_VIRT,
        TEGRA_RAM_PHYS,
        1024,
        PDE_SECTION_HV)

        @ Kernel.
        MAP_ENTRY(DIRECTMAP_VIRT_START,
        DIRECTMAP_PHYS_START,
        (DIRECTMAP_VIRT_END -
        DIRECTMAP_VIRT_START) >> 20,
        PDE_SECTION_HV)

        @ HOST1X
        MAP_ENTRY(TEGRA_HOST1X_VIRT,
        TEGRA_HOST1X_PHYS,
        TEGRA_HOST1X_SIZE >> 20,
        PDE_SECTION_IO)
        
        @ CPU.
        MAP_ENTRY(TEGRA_CPU_VIRT,
        TEGRA_CPU_PHYS,
        TEGRA_CPU_SIZE >> 20,
        PDE_SECTION_IO)

        @ PPSB.
        MAP_ENTRY(TEGRA_PPSB_VIRT,
        TEGRA_PPSB_PHYS,
        TEGRA_PPSB_SIZE >> 20,
        PDE_SECTION_IO)

        @ APB.
        MAP_ENTRY(TEGRA_APB_VIRT,
        TEGRA_APB_PHYS,
        TEGRA_APB_SIZE >> 20,
        PDE_SECTION_IO)

        @ The end.
        MAP_ENTRY(0,0,0,0)
LC0:
        .word   LC0                     @ r1
        .word   text                    @ r2
        .word   _end                    @ r3
        .word   reloc_done              @ r4
        .align

ENTRY(atag_info_ptr)
__ATAG_INFO:
        .long   0

boot_vectors_loc:
        .long boot_vectors

        @ According to ARM ARM, VBAR has lower 5 bits wired to 0.
        .align 5
boot_vectors:
        b dead_loop        @ RESET, never taken
        b boot_und_vector  @ UND
        b boot_swi_vector  @ SWI
        b boot_pabt_vector @ PABT
        b boot_dabt_vector @ DABT
        b boot_unk_vector  @ ?
        b boot_irq_vector  @ IRQ
        b boot_fiq_vector  @ FIQ
boot_und_vector:
        adr     r0, str_und
        b       dead_print_loop
str_und:        .asciz  "\r\Undefined Exception!"
        .align
boot_swi_vector:
        adr     r0, str_swi
        b       dead_print_loop
str_swi:        .asciz  "\r\SWI Exception!"
        .align
boot_pabt_vector:
        adr     r0, str_pabt
        bl      printascii
        adr     r0, str_ifsr
        bl      printascii
        mrc     p15, 0, r0, c5, c0, 1
        bl      printhex8
        adr     r0, str_aifsr
        bl      printascii
        mrc     p15, 0, r0, c5, c1, 1
        bl      printhex8
        adr     r0, str_ifar
        bl      printascii
        mrc     p15, 0, r0, c6, c0, 2
        bl      printhex8
        b       dead_loop
str_pabt:       .asciz  "\r\nPABT Exception!"
str_ifsr:       .asciz  "\r\nIFSR  0x"
str_aifsr:      .asciz  "\r\nAIFSR 0x"
str_ifar:       .asciz  "\r\nIFAR  0x"
        .align
boot_dabt_vector:
        adr     r0, str_dabt
        bl      printascii
        adr     r0, str_dfsr
        bl      printascii
        mrc     p15, 0, r0, c5, c0, 0
        bl      printhex8
        adr     r0, str_adfsr
        bl      printascii
        mrc     p15, 0, r0, c5, c1, 0
        bl      printhex8
        adr     r0, str_dfar
        bl      printascii
        mrc     p15, 0, r0, c6, c0, 0
        bl      printhex8
        b       dead_loop
str_dabt:       .asciz  "\r\DABT Exception!"
str_dfsr:       .asciz  "\r\nDFSR  0x"
str_adfsr:      .asciz  "\r\nADFSR 0x"
str_dfar:       .asciz  "\r\nDFAR  0x"
        .align
boot_unk_vector:
        adr     r0, str_unk
        b       dead_print_loop
str_unk:        .asciz  "\r\Unknown Exception!"
        .align
boot_irq_vector:
        adr     r0, str_irq
        b       dead_print_loop
str_irq:        .asciz  "\r\IRQ Exception!"
        .align
boot_fiq_vector:
        adr     r0, str_fiq
        b       dead_print_loop
str_fiq:        .asciz  "\r\FIQ Exception!"
        .align
dead_print_loop:
        bl      printascii
dead_loop:
1:      b       1b
        
        .section        .bss.stack_aligned,"w"
ENTRY(svc_stack)
        .fill STACK_SIZE,1,0
ENTRY(irq_stack)
        .fill STACK_SIZE,1,0
ENTRY(abt_stack)
        .fill STACK_SIZE,1,0
ENTRY(und_stack)
        .fill STACK_SIZE,1,0
