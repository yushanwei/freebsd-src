/*-
 * Copyright (c) 2015-2018 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 *
 * Portions of this software were developed by SRI International and the
 * University of Cambridge Computer Laboratory under DARPA/AFRL contract
 * FA8750-10-C-0237 ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Portions of this software were developed by the University of Cambridge
 * Computer Laboratory as part of the CTSRD Project, with support from the
 * UK Higher Education Innovation Fund (HEIF).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "assym.inc"

#include <machine/asm.h>
#include <machine/cpu.h>
#include <machine/param.h>
#include <machine/pte.h>
#include <machine/loongarchreg.h>

	.globl	kernbase
	.set	kernbase, KERNBASE

	.text
/*
 * Main entry point. This routine is marked as the ELF entry, and is where
 * loader(8) will enter the kernel. Arguments are as follows:
 *  - a0 = modulep
 *  - a1 = ???
 *
 * It is expected that only a single CPU will enter here.
 */
	.globl _start
_start:
	/*
	 * Zero a1 to indicate that we have no DTB pointer. It is already
	 * included in the loader(8) metadata.
	 */
	move		a1, zero

	/*
	 *  a0 - modulep or zero
	 *  a1 - zero or dtbp
	 */

	/* disable PG */
	LDI		t0, 0xa8		# PLV=0, IE=0, PG=0, DA=1
	csrwr		t0, LOONGARCH_CSR_CRMD

pagetables:
	/* Get the kernel's load address in s1 */
	bl		get_physmem

	/*
         * map a 1GB region starting at KERNBASE using 2MB hugepage
	 */
	la.pcrel	$t0, pagetable_l1
	la.pcrel	$t1, pagetable_l2
	LDI		$t2, KERNBASE
	move		$t3, $s1
	LDI		$t4, L1_SIZE
	bl		create_pagetables_to_l2

	/*
         * map a 1GB region starting at 0 using 2MB hugepage, Lower half
	 */
	la.pcrel	$t0, pagetable_id_l1
	la.pcrel	$t1, pagetable_id_l2
	LDI		$t2, 0			/* start from virt 0 */
	LDI		$t3, 0			/* start phys 0 */
	LDI		$t4, L1_SIZE
	bl		create_pagetables_to_l2

	/*
         * map devmap L1 pagetable
	 */
	la.pcrel	$t0, pagetable_l1
	la.pcrel	$t1, pagetable_l2_devmap
	LDI		$t2, (VM_MAX_KERNEL_ADDRESS - PMAP_MAPDEV_EARLY_SIZE)
	bl		create_pagetables_to_l1

	/* setup 4KB page size */
	LDI		$t0, 0xc
	csrwr		$t0, LOONGARCH_CSR_STLBPGSIZE

	/* setup TLBIDX page size */
	LDI		$t0, (0xc << CSR_TLBIDX_PS_SHIFT)
	csrwr		$t0, LOONGARCH_CSR_TLBIDX

	/* set TLBREHI page size */
	LDI		$t0, 0xc
	csrwr		$t0, LOONGARCH_CSR_TLBREHI

	/* set PWCL */
	LDI		$t0, PWCL_BOOT
	csrwr		$t0, LOONGARCH_CSR_PWCTL0

	/* set PWCH */
	LDI		$t0, PWCH_BOOT
	csrwr		$t0, LOONGARCH_CSR_PWCTL1

	/* setup PGD */
	la.pcrel	$t0, pagetable_l1
	csrwr		$t0, LOONGARCH_CSR_PGDH		/* high half */
	la.pcrel	$t0, pagetable_id_l1
	csrwr		$t0, LOONGARCH_CSR_PGDL		/* low half identity map*/

	/* setup tlbrentry */
	la.pcrel	$t0, handle_tlbrefill_boot
	csrwr		$t0, LOONGARCH_CSR_TLBRENTRY

	/* record virtdone virt addr*/
	la.pcrel	$t1, Lvirtdone
	LDI		$t5, $t1, 0

	/* Enable PG */
        LDI            $t0, 0xb0                # PLV=0, IE=0, PG=1
        csrwr           $t0, LOONGARCH_CSR_CRMD
        LDI            $t0, 0x04                # PLV=0, PIE=1, PWE=0
        csrwr           $t0, LOONGARCH_CSR_PRMD
        LDI            $t0, 0x00                # FPE=0, SXE=0, ASXE=0, BTE=0
        csrwr           $t0, LOONGARCH_CSR_EUEN

	/* setup exception */
	csrrd		$t0, LOONGARCH_CSR_ECFG
	LDI		$t1, ~0x70000
	and		$t0, $t0, $t1
	/* enable timer and ipi */
	LDI		$t1, ((1 << IRQ_TI) | (1 << IRQ_IPI))
	or		$t0, $t0, $t1
	csrwr		$t0, LOONGARCH_CSR_ECFG

	la.pcrel	$t0, cpu_exception_handler
	la.pcrel	$t1, virt_map	/* physical address of virt_map */
	LDI		$t2, $t1, 0	/* virtual address of virt_map */
	SUB		$s1, $t2, $t1	/* calculate phys->virt delta */
	ADD		$t0, $t0, $s1
	csrwr		$t0, LOONGARCH_CSR_EENTRY

	/* flush tlb (global) */
	invtlb		0, $zero, $zero

	/* jump to virtual address space */
	jirl		$zero, $t5, 0

virtdone:
	la.pcrel	$t0, Lbss
        LD		$t0, $t0, 0         # clear .bss
        ST            $zero, $t0, 0
	la.pcrel	$t1, Lend
	LD		$t1, $t1, 0
1:
        ADDI          $t0, $t0, 8
        ST            $zero, $t0, 0
        bne             $t0, $t1, 1b

#ifdef CONFIG_PAGE_SIZE_4KB
        LDI            $t0, 0
        LDI            $t1, CSR_STFILL
        csrxchg         $t0, $t1, LOONGARCH_CSR_IMPCTL1
#endif

        /* KSave3 used for percpu base, initialized as 0 */
        csrwr           $zero, PERCPU_BASE_KS
        /* GPR21 used for percpu base (runtime), initialized as 0 */
        move            $r21, $zero

	la.pcrel	$tp, initstack

	/* Initialize stack pointer */
	la.pcrel	$sp, initstack_end

	/* Allocate space for thread0 PCB and bootparams*/
	LDI		$t0, (-(PCB_SIZE + LOONGARCH_BOOTPARAMS_SIZE) & ~STACKALIGNBYTES)
	ADD		$sp, $sp, $t0

	/* fill bootparams */
	la.pcrel	$t0, initstack
	ST		$t0, $sp, LOONGARCH_BOOTPARAMS_KERN_STACK
	ST		$s1, $sp, LOONGARCH_BOOTPARAMS_KERN_PHYS
	la.pcrel	$t0, pagetable_l1
	ST		$t0, $sp, LOONGARCH_BOOTPARAMS_KERN_L1PT
	ST		$a0, $sp, LOONGARCH_BOOTPARAMS_MODULEP

	move	$a0, $sp
	bl	_C_LABEL(initloongarch) /* Off we go */
	bl	_C_LABEL(mi_startup)

	/* We should never reach here, but if so just hang. */
2:
	idle	0
	b	2b

/*
 * Get the physical address the kernel is loaded to. Returned in s1.
 */
get_physmem:
	la.pcrel	$t0, virt_map	/* physical address of virt_map */
	LD	$t1, $t0, 0	/* virtual address of virt_map */
	SUB	$t1, $t1, $t0	/* calculate phys->virt delta */
	LD	$t2, KERNBASE
	SUB	$s1, $t2, $t1	/* s1 = physmem base */
	jr	$ra

/*
 * Create L1 pagetables
 * @parms
 * t0: l1 address
 * t1: l2 address
 * t2: virtual adress
 */
create_pagetables_to_l1:
	/* level 1 */
	SRLI		$a2, $t1, PAGE_SHIFT

	SRLI		$a3, $t2, L1_SHIFT
	andi		$a3, $a3, Ln_ADDR_MASK	/* L1 index */

	SLLI		$t5, $a2, PPNS		/* L1 pte */

	LD		$t6, PTE_SIZE
	MUL		$a3, $a3, $t6		/* L1 slot */
	STX		$t5, $t0, $a3		/* save L1 pte */
	jr		$ra

/*
 * Create L1 and L2 pagetables use 2MB huge page
 * @parms
 * t0: l1 address
 * t1: l2 address
 * t2: virtual adress
 * t3: physical address
 * t4: map size
 */
create_pagetables_to_l2:
	/* level 1 */
	SRLI		$a2, $t1, PAGE_SHIFT

	SRLI		$a3, $t2, L1_SHIFT
	andi		$a3, $a3, Ln_ADDR_MASK	/* L1 index */

	SLLI		$t5, $a2, PPNS		/* L1 pte */

	LD		$t6, PTE_SIZE
	MUL		$a3, $a3, $t6		/* L1 slot */
	STX		$t5, $t0, $a3		/* save L1 pte */

	/* level 2 huge page  */
	SRLI		$t5, $t2, L2_SHIFT
	andi		$t5, $t5, Ln_ADDR_MASK	/* L2 index */
	SRLI		$t6, $t4, L2_SHIFT	/* L2 count */
	ADD		$t6, $t6, $t5		/* t6 => map end idex */

	LD		$a2, PMD_KERN_HUGE	/* pte entry */
	LD		$a3, PTE_SIZE
	MUL		$a4, $t5, $a3		/* L2 slot */
	SRLI		$t4, $t3, L2_SHIFT
1:
	SLLI		$a3, $t4, PPNS_HUGE_2M
	or		$a3, $a3, $a2		/* meke L2 pte */
	STX		$a3, $t1, $a4		/* save L2 pte */

	ADDI		$a4, $a4, PTE_SIZE	/* update l2 slot */
	ADDI		$t4, $t4, 1		/* update physical address */
	ADDI		$t5, $t5, 1		/* update l2 index */
	bltu		$t5, $t6, 1b		/* build all 512 entries */
	jr		$ra

handle_tlbrefill_boot:
	csrwr	$t0, LOONGARCH_CSR_TLBRSAVE
	csrrd	$t0, LOONGARCH_CSR_PGD
	lddir	$t0, $t0, 2
	lddir	$t0, $t0, 1
	ldpte	$t0, 0
	ldpte	$t0, 1
	tlbfill
	csrrd	$t0, LOONGARCH_CSR_TLBRSAVE
	ertn

	.align  4
initstack:
	.space  (PAGE_SIZE * KSTACK_PAGES)
initstack_end:

ENTRY(sigcode)
	move    $a0, $sp
	ADDI  $a0, $a0, SF_UC

1:
        LD    $a7, SYS_sigreturn
        syscall	0

        /* sigreturn failed, exit */
        LD    $a7, SYS_exit
        syscall	0

        b       1b
END(sigcode)
        /* This may be copied to the stack, keep it 16-byte aligned */
        .align  3
esigcode:

        .data
        .align  3
        .global szsigcode
szsigcode:
        .quad   esigcode - sigcode

        .align  12
pagetable_l1:
        .space  PAGE_SIZE
pagetable_l2:
        .space  PAGE_SIZE
pagetable_l2_devmap:
        .space  PAGE_SIZE
pagetable_id_l1:
	.space	PAGE_SIZE
pagetable_id_l2:
	.space	PAGE_SIZE

        .align 3

virt_map:
	.quad   virt_map
Lvirtdone:
	.quad	virtdone
Lbss:
	.quad	__bss_start
Lend:
	.quad	_end

	.globl init_pt_va
init_pt_va:
	.quad pagetable_l2	/* XXX: Keep page tables VA */

#ifndef SMP
ENTRY(mpentry)
1:
	idle 	0
	j	1b
END(mpentry)
#else
/*
 * mpentry(unsigned long)
 *
 * Called by a core when it is being brought online.
 */
ENTRY(mpentry)

	/* disable PG */
	LDI		$t0, 0xa8		# PLV=0, IE=0, PG=0, DA=1
	csrwr		$t0, LOONGARCH_CSR_CRMD

	/* setup 4KB page size */
	LDI		$t0, 0xc
	csrwr		$t0, LOONGARCH_CSR_STLBPGSIZE

	/* setup TLBIDX page size */
	LDI		$t0, (0xc << CSR_TLBIDX_PS_SHIFT)
	csrwr		$t0, LOONGARCH_CSR_TLBIDX

	/* set TLBREHI page size */
	LDI		$t0, 0xc
	csrwr		$t0, LOONGARCH_CSR_TLBREHI

	/* set PWCL */
	LDI		$t0, PWCL_BOOT
	csrwr		$t0, LOONGARCH_CSR_PWCTL0

	/* set PWCH */
	LDI		$t0, PWCH_BOOT
	csrwr		$t0, LOONGARCH_CSR_PWCTL1

	/* setup PGD */
	la.pcrel	$t0, pagetable_l1
	csrwr		$t0, LOONGARCH_CSR_PGDH		/* high half */
	la.pcrel	$t0, pagetable_id_l1
	csrwr		$t0, LOONGARCH_CSR_PGDL		/* low half identity map*/

	/* setup tlbrentry */
	la.pcrel	$t0, handle_tlbrefill_boot
	csrwr		$t0, LOONGARCH_CSR_TLBRENTRY

	/* record virtdone virt addr*/
	la.pcrel	$t1, LSvirtdone
	LD		$t5, $t1, 0

	/* Enable PG */
        LDI            $t0, 0xb0                # PLV=0, IE=0, PG=1
        csrwr           $t0, LOONGARCH_CSR_CRMD
        LDI            $t0, 0x04                # PLV=0, PIE=1, PWE=0
        csrwr           $t0, LOONGARCH_CSR_PRMD
        LDI            $t0, 0x00                # FPE=0, SXE=0, ASXE=0, BTE=0
        csrwr           $t0, LOONGARCH_CSR_EUEN

	/* setup exception */
	csrrd		$t0, LOONGARCH_CSR_ECFG
	LDI		$t1, ~0x70000
	and		$t0, $t0, $t1
	/* enable timer and ipi */
	LDI		$t1, ((1 << IRQ_TI) | (1 << IRQ_IPI))
	or		$t0, $t0, $t1
	csrwr		$t0, LOONGARCH_CSR_ECFG

	la.pcrel	$t0, cpu_exception_handler
	la.pcrel	$t1, virt_map	/* physical address of virt_map */
	LD		$t2, $t1, 0	/* virtual address of virt_map */
	SUB 		$s1, $t2, $t1	/* calculate phys->virt delta */
	SUB 		$t0, $t0, $s1
	csrwr		$t0, LOONGARCH_CSR_EENTRY

	/* flush tlb (global) */
	invtlb		0, $zero, $zero

	/* jump to virtual address space */
	jirl		$zero, $t5, 0


svirtdone:

	/* get cpuid, pass to init_secondary */
	csrrd	$a0, LOONGARCH_CSR_CPUID
	andi	$a0, $a0, 0x3ff

	/*
	 * Calculate the offset to __loongarch_boot_ap
	 * for the current core, cpuid is in a0.
	 */
	LDI	$t1, 4
	MUL	$t1, $t1, $a0
	/* Get the pointer */
	la.pcrel	$t0, __loongarch_boot_ap
	ADD	$t0, $t0, $t1

1:
	/* Wait the kernel to be ready */
	LD	$t1, $t0, 0
	beqz	$t1, $1b

	/* Setup stack pointer */
	la.pcrel	$t0, bootstack
	LD	$sp, $t0, 0

	bl	init_secondary
END(mpentry)
#endif

        .align 3
LSvirtdone:
	.quad	svirtdone
