/*-
 * Copyright (c) 2014 Andrew Turner
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

#ifndef _MACHINE_PTE_H_
#define	_MACHINE_PTE_H_

#ifndef LOCORE
typedef	uint64_t	pd_entry_t;		/* page directory entry */
typedef	uint64_t	pt_entry_t;		/* page table entry */
typedef	uint64_t	pn_t;			/* page number */
#endif

/* Level 0 table, 512GiB per entry, SV48 only */
#define	L0_SHIFT	39
#define	L0_SIZE		(1UL << L0_SHIFT)
#define	L0_OFFSET	(L0_SIZE - 1)

/* Level 1 table, 1GiB per entry */
#define	L1_SHIFT	30
#define	L1_SIZE 	(1UL << L1_SHIFT)
#define	L1_OFFSET 	(L1_SIZE - 1)

/* Level 2 table, 2MiB per entry */
#define	L2_SHIFT	21
#define	L2_SIZE 	(1UL << L2_SHIFT)
#define	L2_OFFSET 	(L2_SIZE - 1)

/* Level 3 table, 4KiB per entry */
#define	L3_SHIFT	12
#define	L3_SIZE 	(1UL << L3_SHIFT)
#define	L3_OFFSET 	(L3_SIZE - 1)

#define	Ln_ENTRIES_SHIFT 9
#define	Ln_ENTRIES	(1 << Ln_ENTRIES_SHIFT)
#define	Ln_ADDR_MASK	(Ln_ENTRIES - 1)

/* Used by software */
#define PTE_PRESENT	(1 << 7)
#define PTE_PRESENT_INVALID	(1 << 60)
#define PTE_WRITE	(1 << 8)
#define PTE_ACCESSED	(1 << 0)
#define PTE_MODIFIED	(1 << 9)
#define PTE_PROTNONE	(1 << 10)
#define PTE_SPECIAL	(1 << 11)
#define PTE_DEVMAP	(1 << 59)

#define PTE_SWP_EXCLUSIVE     (1 << 23)

/* Used by hardware */
#define	PTE_VALID	(1 << 0) /* Valid */
#define PTE_DIRTY	(1 << 1) /* Dirty */
#define PTE_PLV_MASK	(3 << 2)
#define PTE_CACHE_MASK	(3 << 4)
#define PTE_GLOBAL	(1 << 6) /* Global */
#define PTE_HUGE	(1 << 6) /* HUGE is a PMD bit */
#define PTE_HGLOBAL	(1 << 12) /* HGlobal is a PMD bit */
#define PTE_NO_READ	(1UL << 61) /* Non Read */
#define PTE_NO_EXEC	(1UL << 62) /* Non Execute */
#define PTE_RPLV	(1UL << 63)

#define PTE_R		(PTE_VALID)
#define PTE_RW          (PTE_DIRTY | PTE_WRITE)
#define PLV_KERN	0
#define PLV_USER	3
#define PLV_MASK	0x3
#define PTE_KERN        (PLV_KERN << 2)
#define PTE_USER        (PLV_USER << 2)
#define PTE_HI_MASK	0xFFC0000000000000ULL

#define	PTE_PPN0_S	10
#define	PTE_PPN1_S	19
#define	PTE_PPN2_S	28
#define	PTE_PPN3_S	37
#define	PTE_SIZE	8

#endif /* !_MACHINE_PTE_H_ */
