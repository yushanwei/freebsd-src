/*-
 * Copyright (c) 2015-2016 Ruslan Bukin <br@bsdpad.com>
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

#include <machine/asm.h>
#include <machine/setjmp.h>

ENTRY(_setjmp)
        ST ra, a0, 0*SZREG
        ST sp, a0, 1*SZREG
	ST u0, a0, 2*SZREG
        ST fp, a0, 3*SZREG
        ST s0, a0, 4*SZREG
        ST s1, a0, 5*SZREG
        ST s2, a0, 6*SZREG
        ST s3, a0, 7*SZREG
        ST s4, a0, 8*SZREG
        ST s5, a0, 9*SZREG
        ST s6, a0, 10*SZREG
        ST s7, a0, 11*SZREG
        ST s8, a0, 12*SZREG
#ifndef __loongarch_soft_float
        FST fs0, a0, 13*SZREG + 0*SZFREG
        FST fs1, a0, 13*SZREG + 1*SZFREG
        FST fs2, a0, 13*SZREG + 2*SZFREG
        FST fs3, a0, 13*SZREG + 3*SZFREG
        FST fs4, a0, 13*SZREG + 4*SZFREG
        FST fs5, a0, 13*SZREG + 5*SZFREG
        FST fs6, a0, 13*SZREG + 6*SZFREG
        FST fs7, a0, 13*SZREG + 7*SZFREG
#endif

  move   a0, zero
  jr     ra
END(_setjmp)

ENTRY(_longjmp)
        LD ra, a0, 0*SZREG
        LD sp, a0, 1*SZREG
	LD u0, a0, 2*SZREG
        LD fp, a0, 3*SZREG
        LD s0, a0, 4*SZREG
        LD s1, a0, 5*SZREG
        LD s2, a0, 6*SZREG
        LD s3, a0, 7*SZREG
        LD s4, a0, 8*SZREG
        LD s5, a0, 9*SZREG
        LD s6, a0, 10*SZREG
        LD s7, a0, 11*SZREG
        LD s8, a0, 12*SZREG

#ifndef __loongarch_soft_float
        FLD fs0, a0, 13*SZREG + 0*SZFREG
        FLD fs1, a0, 13*SZREG + 1*SZFREG
        FLD fs2, a0, 13*SZREG + 2*SZFREG
        FLD fs3, a0, 13*SZREG + 3*SZFREG
        FLD fs4, a0, 13*SZREG + 4*SZFREG
        FLD fs5, a0, 13*SZREG + 5*SZFREG
        FLD fs6, a0, 13*SZREG + 6*SZFREG
        FLD fs7, a0, 13*SZREG + 7*SZFREG
#endif
        sltui a0, a1, 1
        ADD a0, a0, a1       # a0 = (a1 == 0) ? 1 : a1
        jirl zero, ra, 0
END(_longjmp)
