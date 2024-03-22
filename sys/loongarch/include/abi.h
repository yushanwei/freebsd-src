/*	$NetBSD: asm.h,v 1.29 2000/12/14 21:29:51 jeffs Exp $	*/

/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)machAsmDefs.h	8.1 (Berkeley) 6/10/93
 *	JNPR: asm.h,v 1.10 2007/08/09 11:23:32 katta
 */

/*
 * machAsmDefs.h --
 *
 *	Macros used when writing assembler programs.
 *
 *	Copyright (C) 1989 Digital Equipment Corporation.
 *	Permission to use, copy, modify, and distribute this software and
 *	its documentation for any purpose and without fee is hereby granted,
 *	provided that the above copyright notice appears in all copies.
 *	Digital Equipment Corporation makes no representations about the
 *	suitability of this software for any purpose.  It is provided "as is"
 *	without express or implied warranty.
 *
 * from: Header: /sprite/src/kernel/mach/ds3100.md/RCS/machAsmDefs.h,
 *	v 1.2 89/08/15 18:28:24 rab Exp  SPRITE (DECWRL)
 */

#ifndef _MACHINE_ABI_H_
#define	_MACHINE_ABI_H_

/*
 * Size of a register
 */
#ifndef __loongarch64
#define SZREG   4
#define SZFREG 4
#define	SZFPREG	4

#define REG_L           ld.w
#define REG_S           st.w
#define REG_ADD         add.w
#define REG_SUB         sub.w
#define BSTRINS         bstrins.w
#define REG_SCALESHIFT  2
#define PTR_ADD         add.w
#define PTR_ADDI        addi.w
#define PTR_SUB         sub.w
#define PTR_L           ld.w
#define PTR_S           st.w
#define PTR_LI          li.w
#define PTR_SLL         slli.w
#define PTR_SLLV        sll.w
#define PTR_SRL         srli.w
#define PTR_SRLV        srl.w
#define PTR_SRA         srai.w
#define PTR_SRAV        sra.w
#define PTR_SCALESHIFT  2
#define PTRSIZE         4
#define PTRLOG          2
#ifdef __ASSEMBLY__
#define PTR_WORD        .word
#endif
#define INT_ADD         add.w
#define INT_ADDI        addi.w
#define INT_SUB         sub.w
#define INT_L           ld.w
#define INT_S           st.w
#define INT_SLL         slli.w
#define INT_SLLV        sll.w
#define INT_SRL         srli.w
#define INT_SRLV        srl.w
#define INT_SRA         srai.w
#define INT_SRAV        sra.w
#ifdef __ASSEMBLY__
#define INT_WORD        .word
#endif
#define INT_SCALESHIFT  2
#define LONG_ADD        add.w
#define LONG_ADDI       addi.w
#define LONG_SUB        sub.w
#define LONG_L          ld.w
#define LONG_S          st.w
#define LONG_SLL        slli.w
#define LONG_SLLV       sll.w
#define LONG_SRL        srli.w
#define LONG_SRLV       srl.w
#define LONG_SRA        srai.w
#define LONG_SRAV       sra.w
#define LONGSIZE        4
#define LONGMASK        3
#define LONGLOG         2
#define LONG_SCALESHIFT 2
#ifdef __ASSEMBLY__
#define LONG            .word
#endif

#else /* __loongarch64 */

#define SZREG   8
#define SZFREG 8
#define	SZFPREG	8

#define REG_L           ld.d
#define REG_S           st.d
#define REG_ADD         add.d
#define REG_SUB         sub.d
#define BSTRINS         bstrins.d
#define REG_SCALESHIFT  3
#define PTR_ADD         add.d
#define PTR_ADDI        addi.d
#define PTR_SUB         sub.d
#define PTR_L           ld.d
#define PTR_S           st.d
#define PTR_LI          li.d
#define PTR_SLL         slli.d
#define PTR_SLLV        sll.d
#define PTR_SRL         srli.d
#define PTR_SRLV        srl.d
#define PTR_SRA         srai.d
#define PTR_SRAV        sra.d
#define PTR_SCALESHIFT  3
#define PTRSIZE         8
#define PTRLOG          3
#ifdef __ASSEMBLY__
#define PTR_WORD        .dword
#endif
#define INT_ADD         add.d
#define INT_ADDI        addi.d
#define INT_SUB         sub.d
#define INT_L           ld.d
#define INT_S           st.d
#define INT_SLL         slli.d
#define INT_SLLV        sll.d
#define INT_SRL         srli.d
#define INT_SRLV        srl.d
#define INT_SRA         srai.d
#define INT_SRAV        sra.d
#ifdef __ASSEMBLY__
#define INT_WORD        .dword
#endif
#define INT_SCALESHIFT  3
#define LONG_ADD        add.d
#define LONG_ADDI       addi.d
#define LONG_SUB        sub.d
#define LONG_L          ld.d
#define LONG_S          st.d
#define LONG_SLL        slli.d
#define LONG_SLLV       sll.d
#define LONG_SRL        srli.d
#define LONG_SRLV       srl.d
#define LONG_SRA        srai.d
#define LONG_SRAV       sra.d
#define LONGSIZE        8
#define LONGMASK        7
#define LONGLOG         3
#define LONG_SCALESHIFT 3
#ifdef __ASSEMBLY__
#define LONG            .dword
#endif

#endif/* __loongarch64 */

#if defined(__loongarch_single_float)
#define FREG_L fld.w
#define FREG_S fst.w
#elif defined(__loongarch_double_float)
#define FREG_L fld.d
#define FREG_S fst.d
#endif

#define SZVREG 16
#define SZXREG 32

#define	ALSK	15		/* stack alignment */
#define	ALMASK	-15		/* stack alignment */
#define	STACK_ALIGN	16

/*
 * How to add/sub/load/store/shift C int variables.
*/
#if (__SIZEOF_INT__ == 4)
#elif (__SIZEOF_INT__ == 8)
#endif
/*
 * How to add/sub/load/store/shift C long variables.
 */
#if (__SIZEOF_LONG__ == 4)
#elif(__SIZEOF_LONG__ == 8)
#endif


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

#endif /* !_MACHINE_ABI_H_ */
