/*-
 * Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.
 * Copright (c) 2021 Loongson Co.
 * Author: Zhou Mingtao <zhoumingtao@loongson.cn>
 *
 * This program and the accompanying materials are licensed and made
 * available
 * under the terms and conditions of the BSD License which accompanies
 * this
 * distribution.  The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php.
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
 * BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED.
 */

#include <machine/asm.h>
#include <machine/setjmp.h>
	.text
	.p2align 3
/*
   int setjmp(jmp_buf env);
*/
ENTRY(_setjmp)
  ST   ra,  a0, 0x00
  ST   sp,  a0, 0x08
  ST   fp,  a0, 0x10
  ST   s0,  a0, 0x18
  ST   s1,  a0, 0x20
  ST   s2,  a0, 0x28
  ST   s3,  a0, 0x30
  ST   s4,  a0, 0x38
  ST   s5,  a0, 0x40
  ST   s6,  a0, 0x48
  ST   s7,  a0, 0x50
  ST   s8,  a0, 0x58
  move   a0, zero
  jr     ra
END(_setjmp)

/*
   void longjmp(jmp_buf env, int val);
*/
ENTRY(_longjmp)
  LD   ra,  a0, 0x00
  LD   sp,  a0, 0x08
  LD   fp,  a0, 0x10
  LD   s0,  a0, 0x18
  LD   s1,  a0, 0x20
  LD   s2,  a0, 0x28
  LD   s3,  a0, 0x30
  LD   s4,  a0, 0x38
  LD   s5,  a0, 0x40
  LD   s6,  a0, 0x48
  LD   s7,  a0, 0x50
  LD   s8,  a0, 0x58
  INT_ADDI a0, zero, 1    # a0 = 1
  beqz a1, .L0           # if (a1 == 0); goto L0
  move a0, a1           # a0 = a1
.L0:
  jr ra
END(_longjmp)
