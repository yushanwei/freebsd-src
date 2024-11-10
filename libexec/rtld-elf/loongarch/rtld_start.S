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
 *	.cfi_undefined ra	 Do not attempt to unwind any further.
 */

ENTRY(.rtld_start)
	move	s0, a0		/* Put ps_strings in a callee-saved register */
	move	s1, sp		/* And the stack pointer */

	ADDI	sp, sp, -16	/* Make room for obj_main & exit proc */

	move	a1, sp		/* exit_proc */
	ADDI	a2, a1, 8	/* obj_main */
	bl	_rtld		/* Call the loader */
	move	t0, a0		/* Backup the entry point */

	LD	a2, sp, 0	/* Load cleanup */
	LD	a1, sp, 8	/* Load obj_main */
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
	ADDI	sp, sp, -(8 * 18)
        ST   a0, sp,(8 * 0)
        ST   a1, sp,(8 * 1)
        ST   a2, sp,(8 * 2)
        ST   a3, sp,(8 * 3)
        ST   a4, sp,(8 * 4)
        ST   a5, sp,(8 * 5)
        ST   a6, sp,(8 * 6)
        ST   a7, sp,(8 * 7)
        ST   ra, sp,(8 * 8)

#ifdef __loongarch_double_float
	/* Save any floating-point arguments */
        FST  fa0, sp,(8 * 9)
        FST  fa1, sp,(8 * 10)
        FST  fa2, sp,(8 * 11)
        FST  fa3, sp,(8 * 12)
        FST  fa4, sp,(8 * 13)
        FST  fa5, sp,(8 * 14)
        FST  fa6, sp,(8 * 15)
        FST  fa7, sp,(8 * 16)
#endif

	/* Reloc offset is 3x of the .got.plt offset */
	SLLI	a1, t1, 1	/* Mult items by 2 */
	ADD	a1, a1, t1	/* Plus item */

	/* Load obj */
	move	a0, t0

	/* Call into rtld */
	bl	_rtld_bind

	/* Backup the address to branch to */
	move	t0, a0

	/* Restore the arguments and ra */
        LD   a0, sp,(8 * 0)
        LD   a1, sp,(8 * 1)
        LD   a2, sp,(8 * 2)
        LD   a3, sp,(8 * 3)
        LD   a4, sp,(8 * 4)
        LD   a5, sp,(8 * 5)
        LD   a6, sp,(8 * 6)
        LD   a7, sp,(8 * 7)
        LD   ra, sp,(8 * 8)

#ifdef __loongarch_double_float
	/* Restore floating-point arguments */
        FLD  fa0, sp,(8 * 9)
        FLD  fa1, sp,(8 * 10)
        FLD  fa2, sp,(8 * 11)
        FLD  fa3, sp,(8 * 12)
        FLD  fa4, sp,(8 * 13)
        FLD  fa5, sp,(8 * 14)
        FLD  fa6, sp,(8 * 15)
        FLD  fa7, sp,(8 * 16)
#endif
	ADDI	sp, sp, (8 * 18)

	/* Call into the correct function */
	jr	t0
END(_rtld_bind_start)
