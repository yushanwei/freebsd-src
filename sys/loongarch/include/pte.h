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
/* Block and Page attributes */
#define ATTR_MASK_H             UINT64_C(0xfffc000000000000)
#define ATTR_MASK_L             UINT64_C(0x0000000000000fff)
#define ATTR_MASK               (ATTR_MASK_H | ATTR_MASK_L)

#define BASE_MASK               ~ATTR_MASK
#define BASE_ADDR(x)            ((x) & BASE_MASK)

#define PTE_TO_PHYS(pte)        BASE_ADDR(pte)
/* Convert a phys addr to the output address field of a PTE */
#define PHYS_TO_PTE(pa)         (pa)


/* Level 0 table, 512GiB per entry, SV48 only */
#define	L0_SHIFT	39
#define	L0_SIZE		(1UL << L0_SHIFT)
#define	L0_OFFSET	(L0_SIZE - 1)
#define L0_INVAL        0x0 /* An invalid address */
        /* 0x1 Level 0 doesn't support block translation */
        /* 0x2 also marks an invalid address */
#define L0_TABLE        0x3 /* A next-level table */


/* Level 1 table, 1GiB per entry */
#define	L1_SHIFT	30
#define	L1_SIZE 	(1UL << L1_SHIFT)
#define	L1_OFFSET 	(L1_SIZE - 1)
#define L1_INVAL        L0_INVAL
#define L1_BLOCK        0x1
#define L1_TABLE        L0_TABLE

/* Level 2 table, 2MiB per entry */
#define	L2_SHIFT	21
#define	L2_SIZE 	(1UL << L2_SHIFT)
#define	L2_OFFSET 	(L2_SIZE - 1)
#define L2_INVAL        L1_INVAL
#define L2_BLOCK        0x1
#define L2_TABLE        L1_TABLE


/* Level 3 table, 4KiB per entry */
#define	L3_SHIFT	12
#define	L3_SIZE 	(1UL << L3_SHIFT)
#define	L3_OFFSET 	(L3_SIZE - 1)
#define L3_INVAL        0x0
        /* 0x1 is reserved */
        /* 0x2 also marks an invalid address */
#define L3_PAGE         0x3

#define L0_ENTRIES_SHIFT 9
#define	Ln_ENTRIES_SHIFT 9
#define	Ln_ENTRIES	(1 << Ln_ENTRIES_SHIFT)
#define	Ln_ADDR_MASK	(Ln_ENTRIES - 1)
#define Ln_TABLE_MASK   ((1 << 12) - 1)

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

/*
 * Svpbmt Memory Attribute (MA) bits [62:61].
 *
 * +------+-------+------------------------------------------------------------+
 * | Mode | Value | Requested Memory Attributes                                |
 * +------+-------+------------------------------------------------------------+
 * | PMA  | 00    | None, inherited from Physical Memory Attributes (firmware) |
 * | NC   | 01    | Non-cacheable, idempotent, weakly-ordered (RVWMO),         |
 * |      |       | main memory                                                |
 * | IO   | 10    | Non-cacheable, non-idempotent, strongly-ordered, I/O       |
 * | --   | 11    | Reserved                                                   |
 * +------+-------+------------------------------------------------------------+

 *#define	PTE_MA_SHIFT		61
 *#define	PTE_MA_MASK		(0x3ul << PTE_MA_SHIFT)
 *#define	PTE_MA_NONE		(0ul)
 *#define	PTE_MA_NC		(1ul << PTE_MA_SHIFT)
 *#define	PTE_MA_IO		(2ul << PTE_MA_SHIFT)
 */

#define PTE_R		(PTE_VALID)
#define PTE_RW          (PTE_DIRTY | PTE_WRITE)
#define PLV_KERN	0
#define PLV_USER	3
#define PLV_MASK	0x3
#define PTE_KERN        (PLV_KERN << 2)
#define PTE_USER        (PLV_USER << 2)

#define PTE_HI_MASK                     0xffff000000000000ULL

#define PTE_PPN0_S      10
#define PTE_PPN1_S      19
#define PTE_PPN2_S      28
#define PTE_PPN3_S      37
#define PTE_SIZE        8

#define L0_ENTRIES_SHIFT 9
#define Ln_ENTRIES_SHIFT 9

#define L0_ENTRIES      (1 << L0_ENTRIES_SHIFT)
#define L0_ADDR_MASK    (L0_ENTRIES - 1)

#define Ln_ENTRIES      (1 << Ln_ENTRIES_SHIFT)
#define Ln_ADDR_MASK    (Ln_ENTRIES - 1)
#define Ln_TABLE_MASK   ((1 << 12) - 1)

/*
 * The number of contiguous Level 3 entries (with ATTR_CONTIGUOUS set) that
 * can be coalesced into a single TLB entry
 */
#define L2C_ENTRIES     16
#define L3C_ENTRIES     16
        
#define L2C_SIZE        (L2C_ENTRIES * L2_SIZE)
#define L2C_OFFSET      (L2C_SIZE - 1)

#define L3C_SIZE        (L3C_ENTRIES * L3_SIZE)
#define L3C_OFFSET      (L3C_SIZE - 1)


#define pmap_l0_index(va)       (((va) >> L0_SHIFT) & L0_ADDR_MASK)
#define pmap_l1_index(va)       (((va) >> L1_SHIFT) & Ln_ADDR_MASK)
#define pmap_l2_index(va)       (((va) >> L2_SHIFT) & Ln_ADDR_MASK)
#define pmap_l3_index(va)       (((va) >> L3_SHIFT) & Ln_ADDR_MASK)

#endif /* !_MACHINE_PTE_H_ */
