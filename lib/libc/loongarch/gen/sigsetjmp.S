/*-
 * Copyright (c) 2015 Ruslan Bukin <br@bsdpad.com>
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

ENTRY(sigsetjmp)
#ifdef PTR_MANGLE
        PTR_MANGLE (t0, ra, t1)
        REG_S t0, a0, 0*SZREG
        PTR_MANGLE2 (t0, sp, t1)
        REG_S t0, a0, 1*SZREG
#else
        REG_S ra, a0, 0*SZREG
        REG_S sp, a0, 1*SZREG
#endif
        REG_S $r21,  a0, 2*SZREG
        REG_S fp, a0, 3*SZREG
        REG_S s0, a0, 4*SZREG
        REG_S s1, a0, 5*SZREG
        REG_S s2, a0, 6*SZREG
        REG_S s3, a0, 7*SZREG
        REG_S s4, a0, 8*SZREG
        REG_S s5, a0, 9*SZREG
        REG_S s6, a0, 10*SZREG
        REG_S s7, a0, 11*SZREG
        REG_S s8, a0, 12*SZREG

#ifndef __loongarch_soft_float
        FREG_S $f24, a0, 13*SZREG + 0*SZFREG
        FREG_S $f25, a0, 13*SZREG + 1*SZFREG
        FREG_S $f26, a0, 13*SZREG + 2*SZFREG
        FREG_S $f27, a0, 13*SZREG + 3*SZFREG
        FREG_S $f28, a0, 13*SZREG + 4*SZFREG
        FREG_S $f29, a0, 13*SZREG + 5*SZFREG
        FREG_S $f30, a0, 13*SZREG + 6*SZFREG
        FREG_S $f31, a0, 13*SZREG + 7*SZFREG
#endif
        b               __sigjmp_save
END(sigsetjmp)

ENTRY(siglongjmp)
#ifdef PTR_MANGLE
        PTR_MANGLE (t0, ra, t1)
        REG_S t0, a0, 0*SZREG
        PTR_MANGLE2 (t0, sp, t1)
        REG_S t0, a0, 1*SZREG
#else
        REG_S ra, a0, 0*SZREG
        REG_S sp, a0, 1*SZREG
#endif
        REG_S $r21,  a0, 2*SZREG
        REG_S fp, a0, 3*SZREG
        REG_S s0, a0, 4*SZREG
        REG_S s1, a0, 5*SZREG
        REG_S s2, a0, 6*SZREG
        REG_S s3, a0, 7*SZREG
        REG_S s4, a0, 8*SZREG
        REG_S s5, a0, 9*SZREG
        REG_S s6, a0, 10*SZREG
        REG_S s7, a0, 11*SZREG
        REG_S s8, a0, 12*SZREG

#ifndef __loongarch_soft_float
        FREG_S $f24, a0, 13*SZREG + 0*SZFREG
        FREG_S $f25, a0, 13*SZREG + 1*SZFREG
        FREG_S $f26, a0, 13*SZREG + 2*SZFREG
        FREG_S $f27, a0, 13*SZREG + 3*SZFREG
        FREG_S $f28, a0, 13*SZREG + 4*SZFREG
        FREG_S $f29, a0, 13*SZREG + 5*SZFREG
        FREG_S $f30, a0, 13*SZREG + 6*SZFREG
        FREG_S $f31, a0, 13*SZREG + 7*SZFREG
#endif

        b               __sigjmp_save

END(siglongjmp)
