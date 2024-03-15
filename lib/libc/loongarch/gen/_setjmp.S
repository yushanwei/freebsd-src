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
#if __loongarch_grlen == 32
  st.w   ra,  a0, 0
  st.w   sp,  a0, 4
  st.w   $r21,   a0, 8
  st.w   fp,  a0, 12
  st.w   s0,  a0, 16
  st.w   s1,  a0, 20
  st.w   s2,  a0, 24
  st.w   s3,  a0, 28
  st.w   s4,  a0, 32
  st.w   s5,  a0, 36
  st.w   s6,  a0, 40
  st.w   s7,  a0, 44
  st.w   s8,  a0, 48
#else /* loongarch_grlen == 64 */
  st.d   ra,  a0, 0
  st.d   sp,  a0, 8
  st.d   $r21,   a0, 16
  st.d   fp,  a0, 24
  st.d   s0,  a0, 32
  st.d   s1,  a0, 40
  st.d   s2,  a0, 48
  st.d   s3,  a0, 56
  st.d   s4,  a0, 64
  st.d   s5,  a0, 72
  st.d   s6,  a0, 80
  st.d   s7,  a0, 88
  st.d   s8,  a0, 96
#endif
  move   a0, zero
  jr     ra
END(_setjmp)

ENTRY(_longjmp)
#if __loongarch_grlen == 32
  ld.w   ra,  a0, 0
  ld.w   sp,  a0, 4
  ld.w   $r21,   a0, 8
  ld.w   fp,  a0, 12
  ld.w   s0,  a0, 16
  ld.w   s1,  a0, 20
  ld.w   s2,  a0, 24
  ld.w   s3,  a0, 28
  ld.w   s4,  a0, 32
  ld.w   s5,  a0, 36
  ld.w   s6,  a0, 40
  ld.w   s7,  a0, 44
  ld.w   s8,  a0, 48
#else /* loongarch_grlen == 64 */
  ld.d   ra,  a0, 0
  ld.d   sp,  a0, 8
  ld.d   $r21,   a0, 16
  ld.d   fp,  a0, 24
  ld.d   s0,  a0, 32
  ld.d   s1,  a0, 40
  ld.d   s2,  a0, 48
  ld.d   s3,  a0, 56
  ld.d   s4,  a0, 64
  ld.d   s5,  a0, 72
  ld.d   s6,  a0, 80
  ld.d   s7,  a0, 88
  ld.d   s8,  a0, 96
#endif
  bnez   a1,  1f
  addi.w a1,  a1, 1
1:
  move   a0,  a1
  jr     ra
END(_longjmp)
