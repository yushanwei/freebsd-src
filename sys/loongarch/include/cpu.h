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

#ifndef _MACHINE_CPU_H_
#define	_MACHINE_CPU_H_

#include <machine/atomic.h>
#include <machine/cpufunc.h>
#include <machine/frame.h>

#define TRAPF_PC(tfp)           ((tfp)->tf_regs.era)
#define TRAPF_USERMODE(tfp)     (((tfp)->tf_regs.estat & PLV_MASK) == PLV_USER)

/*
 * As described in LoongArch specs from Loongson Technology, the PRID register
 * (CPUCFG.00) has the following layout:
 *
 * +---------------+----------------+------------+--------------------+
 * | Reserved      | Company ID     | Series ID  |  Product ID        |
 * +---------------+----------------+------------+--------------------+
 * 31            24 23            16 15        12 11                  0
 */

/*
 * Assigned Company values for bits 23:16 of the PRID register.
 */

#define PRID_COMP_MASK    0xff0000

#define PRID_COMP_LOONGSON  0x140000

/*
 * Assigned Series ID values for bits 15:12 of the PRID register. In order
 * to detect a certain CPU type exactly eventually additional registers may
 * need to be examined.
 */

#define PRID_SERIES_MASK  0xf000

#define PRID_SERIES_LA132 0x8000  /* Loongson 32bit */
#define PRID_SERIES_LA264 0xa000  /* Loongson 64bit, 2-issue */
#define PRID_SERIES_LA364 0xb000  /* Loongson 64bit, 3-issue */
#define PRID_SERIES_LA464 0xc000  /* Loongson 64bit, 4-issue */
#define PRID_SERIES_LA664 0xd000  /* Loongson 64bit, 6-issue */


/*
#define	cpu_getstack(td)	((td)->td_frame->tf_sp)
#define	cpu_setstack(td, sp)	((td)->td_frame->tf_sp = (sp))
*/
#define cpu_getstack(td)        ((td)->td_frame->tf_regs.sp)
#define cpu_setstack(td, sp)    ((td)->td_frame->tf_regs.sp = (sp))
#define cpu_spinwait()          /* nothing */
#define cpu_lock_delay()        DELAY(1)
#ifdef _KERNEL

/*
 * Core manufacturer IDs, as reported by the mvendorid CSR.
#define	MVENDORID_UNIMPL	0x0
#define	MVENDORID_SIFIVE	0x489
#define	MVENDORID_THEAD		0x5b7
 */
/*
 * Particular Product ID values for bits 11:0 of the PRID register.
 */

#define PRID_PRODUCT_MASK 0x0fff


#define LOONGARCH_ARCH_MASK 0x3

#if !defined(__ASSEMBLY__)

enum cpu_type_enum {
  CPU_UNKNOWN,
  CPU_LOONGSON32,
  CPU_LOONGSON64,
  CPU_LAST
};

#endif /* !__ASSEMBLY */

/*
 * ISA Level encodings
 *
 */
#define LOONGARCH_CPU_ISA_LA32R 0x00000001
#define LOONGARCH_CPU_ISA_LA32S 0x00000002
#define LOONGARCH_CPU_ISA_LA64  0x00000004

#define LOONGARCH_CPU_ISA_32BIT (LOONGARCH_CPU_ISA_LA32R | LOONGARCH_CPU_ISA_LA32S)
#define LOONGARCH_CPU_ISA_64BIT LOONGARCH_CPU_ISA_LA64


/*
 * CPU Option encodings
 */
#define CPU_FEATURE_CPUCFG    0 /* CPU has CPUCFG */
#define CPU_FEATURE_LAM     1 /* CPU has Atomic instructions */
#define CPU_FEATURE_UAL     2 /* CPU supports unaligned access */
#define CPU_FEATURE_FPU     3 /* CPU has FPU */
#define CPU_FEATURE_LSX     4 /* CPU has LSX (128-bit SIMD) */
#define CPU_FEATURE_LASX    5 /* CPU has LASX (256-bit SIMD) */
#define CPU_FEATURE_CRC32   6 /* CPU has CRC32 instructions */
#define CPU_FEATURE_COMPLEX   7 /* CPU has Complex instructions */
#define CPU_FEATURE_CRYPTO    8 /* CPU has Crypto instructions */
#define CPU_FEATURE_LVZ     9 /* CPU has Virtualization extension */
#define CPU_FEATURE_LBT_X86   10  /* CPU has X86 Binary Translation */
#define CPU_FEATURE_LBT_ARM   11  /* CPU has ARM Binary Translation */
#define CPU_FEATURE_LBT_MIPS    12  /* CPU has MIPS Binary Translation */
#define CPU_FEATURE_TLB     13  /* CPU has TLB */
#define CPU_FEATURE_CSR     14  /* CPU has CSR */
#define CPU_FEATURE_WATCH   15  /* CPU has watchpoint registers */
#define CPU_FEATURE_VINT    16  /* CPU has vectored interrupts */
#define CPU_FEATURE_CSRIPI    17  /* CPU has CSR-IPI */
#define CPU_FEATURE_EXTIOI    18  /* CPU has EXT-IOI */
#define CPU_FEATURE_PREFETCH    19  /* CPU has prefetch instructions */
#define CPU_FEATURE_PMP     20  /* CPU has perfermance counter */
#define CPU_FEATURE_SCALEFREQ   21  /* CPU supports cpufreq scaling */
#define CPU_FEATURE_FLATMODE    22  /* CPU has flat mode */
#define CPU_FEATURE_EIODECODE   23  /* CPU has EXTIOI interrupt pin decode mode */
#define CPU_FEATURE_GUESTID   24  /* CPU has GuestID feature */
#define CPU_FEATURE_HYPERVISOR    25  /* CPU has hypervisor (running in VM) */
#define CPU_FEATURE_PTW     26  /* CPU has hardware page table walker */

#define LOONGARCH_CPU_CPUCFG    1ULL<<CPU_FEATURE_CPUCFG
#define LOONGARCH_CPU_LAM   1ULL<<CPU_FEATURE_LAM
#define LOONGARCH_CPU_UAL   1ULL<<CPU_FEATURE_UAL
#define LOONGARCH_CPU_FPU   1ULL<<CPU_FEATURE_FPU
#define LOONGARCH_CPU_LSX   1ULL<<CPU_FEATURE_LSX
#define LOONGARCH_CPU_LASX    1ULL<<CPU_FEATURE_LASX
#define LOONGARCH_CPU_CRC32   1ULL<<CPU_FEATURE_CRC32
#define LOONGARCH_CPU_COMPLEX   1ULL<<CPU_FEATURE_COMPLEX
#define LOONGARCH_CPU_CRYPTO    1ULL<<CPU_FEATURE_CRYPTO
#define LOONGARCH_CPU_LVZ   1ULL<<CPU_FEATURE_LVZ
#define LOONGARCH_CPU_LBT_X86   1ULL<<CPU_FEATURE_LBT_X86
#define LOONGARCH_CPU_LBT_ARM   1ULL<<CPU_FEATURE_LBT_ARM
#define LOONGARCH_CPU_LBT_MIPS    1ULL<<CPU_FEATURE_LBT_MIPS
#define LOONGARCH_CPU_TLB   1ULL<<CPU_FEATURE_TLB
#define LOONGARCH_CPU_CSR   1ULL<<CPU_FEATURE_CSR
#define LOONGARCH_CPU_WATCH   1ULL<<CPU_FEATURE_WATCH
#define LOONGARCH_CPU_VINT    1ULL<<CPU_FEATURE_VINT
#define LOONGARCH_CPU_CSRIPI    1ULL<<CPU_FEATURE_CSRIPI
#define LOONGARCH_CPU_EXTIOI    1ULL<<CPU_FEATURE_EXTIOI
#define LOONGARCH_CPU_PREFETCH    1ULL<<CPU_FEATURE_PREFETCH
#define LOONGARCH_CPU_PMP   1ULL<<CPU_FEATURE_PMP
#define LOONGARCH_CPU_SCALEFREQ   1ULL<<CPU_FEATURE_SCALEFREQ
#define LOONGARCH_CPU_FLATMODE    1ULL<<CPU_FEATURE_FLATMODE
#define LOONGARCH_CPU_EIODECODE   1ULL<<CPU_FEATURE_EIODECODE
#define LOONGARCH_CPU_GUESTID   1ULL<<CPU_FEATURE_GUESTID
#define LOONGARCH_CPU_HYPERVISOR  1ULL<<CPU_FEATURE_HYPERVISOR
#define LOONGARCH_CPU_PTW   1ULL<<CPU_FEATURE_PTW

/* cache_desc->flags */
enum {
        CACHE_PRESENT   = (1 << 0),
        CACHE_PRIVATE   = (1 << 1),     /* core private cache */
        CACHE_INCLUSIVE = (1 << 2),     /* include the inner level caches */
};

/*
 * MMU virtual-addressing modes. Support for each level implies the previous,
 * so Sv48-enabled systems MUST support Sv39, etc.
 */
#define	MMU_SV39	0x1	/* 3-level paging */
#define	MMU_SV48	0x2	/* 4-level paging */
#define	MMU_SV57	0x4	/* 5-level paging */
/*
 * Descriptor for a cache
 */
struct cache_desc {
        unsigned char type;
        unsigned char level;
        unsigned short sets;    /* Number of lines per set */
        unsigned char ways;     /* Number of ways */
        unsigned char linesz;   /* Size of line in bytes */
        unsigned char flags;    /* Flags describing cache properties */
};

#define CACHE_LEVEL_MAX         3
#define CACHE_LEAVES_MAX        6

struct cpuinfo_loongarch {
        uint64_t                                asid_cache;
        unsigned long           asid_mask;
        uint32_t                                arch;
        uint32_t                                hwcap;

        /*
         * Capability and feature descriptor structure for LoongArch CPU
         */
        unsigned long long      options;
        unsigned int            processor_id;
        unsigned int            fpu_vers;
        unsigned int            fpu_csr0;
        unsigned int            fpu_mask;
        unsigned int            cputype;
        int                     isa_level;
        int                     tlbsize;
        int                     tlbsizemtlb;
        int                     tlbsizestlbsets;
        int                     tlbsizestlbways;
        int                     cache_leaves_present; /* number of cache_leaves[] elements */
        struct cache_desc       cache_leaves[CACHE_LEAVES_MAX];
        int                     core;   /* physical core number in package */
        int                     package;/* physical package number */
        int                     global_id; /* physical global thread number */
        int                     vabits; /* Virtual Address size in bits */
        int                     pabits; /* Physical Address size in bits */
        unsigned int            ksave_mask; /* Usable KSave mask. */
        unsigned int            watch_dreg_count;   /* Number data breakpoints */
        unsigned int            watch_ireg_count;   /* Number instruction breakpoints */
        unsigned int            watch_reg_use_cnt; /* min(NUM_WATCH_REGS, watch_dreg_count + watch_ireg_count), Usable by ptrace */
};

#define __cpucfg(reg) \
({                                                                              \
                uint64_t val;                   \
                __asm __volatile("cpucfg %0, %1" \
                 :"=r" (val) \
                 : "r" (reg) \
                ); \
          val & 0xffffffff; \
})

#define read_cpucfg(reg) __cpucfg(reg)

extern char btext[];
extern char etext[];

void	cpu_halt(void) __dead2;
void	cpu_reset(void) __dead2;
void	fork_trampoline(void);
void	identify_cpu(u_int cpu);
void	printcpuinfo(u_int cpu);

static __inline uint64_t
get_cyclecount(void)
{

	return (rdcycle());
}

#endif

#endif /* !_MACHINE_CPU_H_ */
