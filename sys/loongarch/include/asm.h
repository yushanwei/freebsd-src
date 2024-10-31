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

#ifndef _MACHINE_ASM_H_
#define	_MACHINE_ASM_H_

/* LoongArch Registers */
#define zero    $r0     /* wired zero */
#define ra      $r1     /* return address */
#define tp      $r2
#define sp      $r3     /* stack pointer */
#define a0      $r4     /* argument registers, a0/a1 reused as v0/v1 for return value */
#define a1      $r5
#define a2      $r6
#define a3      $r7
#define a4      $r8
#define a5      $r9
#define a6      $r10
#define a7      $r11
#define t0      $r12    /* caller saved */
#define t1      $r13
#define t2      $r14
#define t3      $r15
#define t4      $r16
#define t5      $r17
#define t6      $r18
#define t7      $r19
#define t8      $r20
#define u0      $r21	 /* Kernel uses it as percpu base */
#define fp      $r22    /* frame pointer */
#define s9      $r22
#define s0      $r23
#define s1      $r24
#define s2      $r25
#define s3      $r26
#define s4      $r27
#define s5      $r28
#define s6      $r29
#define s7      $r30
#define s8      $r31

#define fa0      $f0
#define fa1      $f1
#define fa2      $f2
#define fa3      $f3
#define fa4      $f4
#define fa5      $f5
#define fa6      $f6
#define fa7      $f7
#define ft0      $f8
#define ft1      $f9
#define ft2      $f10
#define ft3      $f11
#define ft4      $f12
#define ft5      $f13
#define ft6      $f14
#define ft7      $f15
#define ft8      $f16
#define ft9      $f17
#define ft10     $f18
#define ft11     $f19
#define ft12     $f20
#define ft13     $f21
#define ft14     $f22
#define ft15     $f23
#define fs0      $f24
#define fs1      $f25
#define fs2      $f26
#define fs3      $f27
#define fs4      $f28
#define fs5      $f29
#define fs6      $f30
#define fs7      $f31

#define vr0	$vr0
#define vr1	$vr1
#define vr2	$vr2
#define vr3	$vr3
#define vr4	$vr4
#define vr5	$vr5
#define vr6	$vr6
#define vr7	$vr7
#define vr8	$vr8
#define vr9	$vr9
#define vr10	$vr10
#define vr11	$vr11
#define vr12	$vr12
#define vr13	$vr13
#define vr14	$vr14
#define vr15	$vr15
#define vr16	$vr16
#define vr17	$vr17
#define vr18	$vr18
#define vr19	$vr19
#define vr20	$vr20
#define vr21	$vr21
#define vr22	$vr22
#define vr23	$vr23
#define vr24	$vr24
#define vr25	$vr25
#define vr26	$vr26
#define vr27	$vr27
#define vr28	$vr28
#define vr29	$vr29
#define vr30	$vr30
#define vr31	$vr31

#define xr0	$xr0
#define xr1	$xr1
#define xr2	$xr2
#define xr3	$xr3
#define xr4	$xr4
#define xr5	$xr5
#define xr6	$xr6
#define xr7	$xr7
#define xr8	$xr8
#define xr9	$xr9
#define xr10	$xr10
#define xr11	$xr11
#define xr12	$xr12
#define xr13	$xr13
#define xr14	$xr14
#define xr15	$xr15
#define xr16	$xr16
#define xr17	$xr17
#define xr18	$xr18
#define xr19	$xr19
#define xr20	$xr20
#define xr21	$xr21
#define xr22	$xr22
#define xr23	$xr23
#define xr24	$xr24
#define xr25	$xr25
#define xr26	$xr26
#define xr27	$xr27
#define xr28	$xr28
#define xr29	$xr29
#define xr30	$xr30
#define xr31	$xr31

#define fcc0	$fcc0
#define fcc1	$fcc1
#define fcc2	$fcc2
#define fcc3	$fcc3
#define fcc4	$fcc4
#define fcc5	$fcc5
#define fcc6	$fcc6
#define fcc7	$fcc7

#define fcsr0   $fcsr0
#define fcsr1   $fcsr1
#define fcsr2   $fcsr2
#define fcsr3   $fcsr3

/*
 * Size of a register
 */
#if __loongarch_grlen == 32
#define SZREG   4
#define SZFREG 4
#define	SZFPREG	4

#define ADD         add.w
#define ADDI        addi.w
#define LD           ld.w
#define LDX          ldx.w
#define LDI          li.w
#define MUL          mul.w
#define MULH         mulh.w
#define MULHU        mulh.wu
#define ST           st.w
#define STX          stx.w
#define SUB         sub.w
#define SLLI         slli.w
#define SRLI         srli.w
#define SRAI         srai.w
#define SLL        sll.w
#define SRL        srl.w
#define SRA        sra.w
#define LL        ll.w
#define SC        sc.w
#define BSTRINS         bstrins.w
#define SCALESHIFT  2
#ifdef __ASSEMBLY__
#define WORD        .word
#endif

#define PTRLOG          2
#define LONGMASK        3
#define LONGLOG         2

#elif __loongarch_grlen == 64

#define SZREG   8
#define SZFREG 8
#define	SZFPREG	8

#define LD	ld.d
#define LDX	ldx.d
#define ST           st.d
#define STX          stx.d
#define ADD         add.d
#define SUB         sub.d
#define ADDI        addi.d
#define LDI          li.d
#define MUL          mul.d
#define MULH         mulh.d
#define MULHU        mulh.du
#define SLLI         slli.d
#define SRLI         srli.d
#define SRAI         srai.d
#define SLL        sll.d
#define SRL        srl.d
#define SRA        sra.d
#define LL	ll.d
#define SC	sc.d
#define BSTRINS         bstrins.d
#define SCALESHIFT  3
#ifdef __ASSEMBLY__
#define WORD        .dword
#endif

#define PTRLOG          3
#define LONGMASK        7
#define LONGLOG         3

#endif/* __loongarch_grlen == 32 */

#if defined(__loongarch_single_float)
#define FLD fld.w
#define FST fst.w
#define FABS fabs.w
#elif defined(__loongarch_double_float)
#define FLD fld.d
#define FST fst.d
#define FABS fabs.d
#endif

#define SZVREG 16
#define SZXREG 32

#define	ALSK	15		/* stack alignment */
#define	ALMASK	-15		/* stack alignment */
#define	STACK_ALIGN	16

/*
#if (__SIZEOF_INT__ == 4)
#elif (__SIZEOF_INT__ == 8)
#endif

#if (__SIZEOF_LONG__ == 4)
#elif(__SIZEOF_LONG__ == 8)
#endif

#if __loongarch_grlen  == 32
#elif longarch_grlen  == 64
#endif
*/

/*
 *  standard callframe {
 *	register_t cf_pad[N];		o32/64 (N=0), n32 (N=1) n64 (N=1)
 *  	register_t cf_args[4];		arg0 - arg3 (only on o32 and o64)
 *  	register_t cf_gp;		global pointer (only on n32 and n64)
 *  	register_t cf_sp;		frame pointer
 *  	register_t cf_ra;		return address
 *  };
 */
#define	CALLFRAME_SIZ	(SZREG * 4)
#define	CALLFRAME_S0	(CALLFRAME_SIZ - 4 * SZREG)
#define	CALLFRAME_GP	(CALLFRAME_SIZ - 3 * SZREG)
#define	CALLFRAME_SP	(CALLFRAME_SIZ - 2 * SZREG)
#define	CALLFRAME_RA	(CALLFRAME_SIZ - 1 * SZREG)

#undef __FBSDID
#if !defined(lint) && !defined(STRIP_FBSDID)
#define	__FBSDID(s)	.ident s
#else
#define	__FBSDID(s)	/* nothing */
#endif /* not lint and not STRIP_FBSDID */

/* LoongArch pref instruction. */
#ifdef CONFIG_CPU_HAS_PREFETCH
#define PREF(hint, addr, offs)                          \
                preld   hint, addr, offs;
#define PREFX(hint, addr, index)                        \
                preldx  hint, addr, index;
#else /* !CONFIG_CPU_HAS_PREFETCH */
#define PREF(hint, addr, offs)
#define PREFX(hint, addr, index)
#endif /* !CONFIG_CPU_HAS_PREFETCH */

#define	_C_LABEL(x)	x

#define	ENTRY(sym)						\
	.text; .globl sym; .type sym,@function; .align 4; sym: .cfi_startproc;
#define	END(sym) .cfi_endproc; .size sym, . - sym

#define	EENTRY(sym)						\
	.globl	sym; sym:
#define	EEND(sym)

#define	WEAK_REFERENCE(sym, alias)				\
	.weak alias;						\
	.set alias,sym

#define	SET_FAULT_HANDLER(handler, tmp)					\
	LD	tmp, tp, PC_CURTHREAD;					\
	LD	tmp, tmp, TD_PCB;		/* Load the pcb */	\
	ST	handler, tmp, PCB_ONFAULT	/* Set the handler */

#define	ENTER_USER_ACCESS(tmp)				\
	LDI	tmp, 0x3;				\
	csrxchg	tmp, tmp, LOONGARCH_CSR_CRMD

#define	EXIT_USER_ACCESS(tmp)				\
	LDI	tmp, 0x3;				\
	csrxchg	zero, tmp, LOONGARCH_CSR_CRMD

#endif /* _MACHINE_ASM_H_ */
