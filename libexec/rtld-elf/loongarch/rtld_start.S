/*-
 * Copyright (c) 2015-2018 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract FA8750-10-C-0237
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * This software was developed by the University of Cambridge Computer
 * Laboratory as part of the CTSRD Project, with support from the UK Higher
 * Education Innovation Fund (HEIF).
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

#include <machine/asm.h>
/*
 * func_ptr_type
 * _rtld(Elf_Addr *sp, func_ptr_type *exit_proc, Obj_Entry **objp)
 */

ENTRY(.rtld_start)
	move	s0, a0		/* Put ps_strings in a callee-saved register */
	move	s1, sp		/* And the stack pointer */

	PTR_ADDI	sp, sp, -16	/* Make room for obj_main & exit proc */

	move	a1, sp		/* exit_proc */
	INT_ADDI	a2, a1, 8	/* obj_main */
	b	_rtld		/* Call the loader */
	move	t0, a0		/* Backup the entry point */

	REG_L	a2, sp,0	/* Load cleanup */
	REG_L	a1, sp,8	/* Load obj_main */
	move	a0, s0		/* Restore ps_strings */
	move	sp, s1		/* Restore the stack pointer */
	jr	t0		/* Jump to the entry point */
END(.rtld_start)

/*
 * t0 = obj pointer
 * t1 = reloc offset
 */
ENTRY(_rtld_bind_start)
	/* Save the arguments and ra */
	/* We require 17 dwords, but the stack must be aligned to 16-bytes */
	PTR_ADDI	sp, sp, -(8 * 18)
	REG_S	a0, sp,(8 * 0)
	REG_S	a1, sp,(8 * 1)
	REG_S	a2, sp,(8 * 2)
	REG_S	a3, sp,(8 * 3)
	REG_S	a4, sp,(8 * 4)
	REG_S	a5, sp,(8 * 5)
	REG_S	a6, sp,(8 * 6)
	REG_S	a7, sp,(8 * 7)
	REG_S	ra, sp,(8 * 8)

#ifdef __loongarch_double_float
	/* Save any floating-point arguments */
	FREG_S	fa0, sp,(8 * 9)
	FREG_S	fa1, sp,(8 * 10)
	FREG_S	fa2, sp,(8 * 11)
	FREG_S	fa3, sp,(8 * 12)
	FREG_S	fa4, sp,(8 * 13)
	FREG_S	fa5, sp,(8 * 14)
	FREG_S	fa6, sp,(8 * 15)
	FREG_S	fa7, sp,(8 * 16)
#endif

	/* Reloc offset is 3x of the .got.plt offset */
	INT_SLL	a1, t1, 1	/* Mult items by 2 */
	INT_ADD	a1, a1, t1	/* Plus item */

	/* Load obj */
	move	a0, t0

	/* Call into rtld */
	b	_rtld_bind

	/* Backup the address to branch to */
	move	t0, a0

	/* Restore the arguments and ra */
	REG_L	a0, sp,(8 * 0)
	REG_L	a1, sp,(8 * 1)
	REG_L	a2, sp,(8 * 2)
	REG_L	a3, sp,(8 * 3)
	REG_L	a4, sp,(8 * 4)
	REG_L	a5, sp,(8 * 5)
	REG_L	a6, sp,(8 * 6)
	REG_L	a7, sp,(8 * 7)
	REG_L	ra, sp,(8 * 8)

#ifdef __loongarch_double_float
	/* Restore floating-point arguments */
	FREG_L	fa0, sp,(8 * 9)
	FREG_L	fa1, sp,(8 * 10)
	FREG_L	fa2, sp,(8 * 11)
	FREG_L	fa3, sp,(8 * 12)
	FREG_L	fa4, sp,(8 * 13)
	FREG_L	fa5, sp,(8 * 14)
	FREG_L	fa6, sp,(8 * 15)
	FREG_L	fa7, sp,(8 * 16)
#endif
	PTR_ADDI	sp, sp, (8 * 18)

	/* Call into the correct function */
	jr	t0
END(_rtld_bind_start)
