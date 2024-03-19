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
  REG_S   ra,  a0, 0x00
  REG_S   sp,  a0, 0x08
  REG_S   fp,  a0, 0x10
  REG_S   s0,  a0, 0x18
  REG_S   s1,  a0, 0x20
  REG_S   s2,  a0, 0x28
  REG_S   s3,  a0, 0x30
  REG_S   s4,  a0, 0x38
  REG_S   s5,  a0, 0x40
  REG_S   s6,  a0, 0x48
  REG_S   s7,  a0, 0x50
  REG_S   s8,  a0, 0x58
  move   a0, zero
  jr     ra
END(_setjmp)

/*
   void longjmp(jmp_buf env, int val);
*/
ENTRY(_longjmp)
  REG_L   ra,  a0, 0x00
  REG_L   sp,  a0, 0x08
  REG_L   fp,  a0, 0x10
  REG_L   s0,  a0, 0x18
  REG_L   s1,  a0, 0x20
  REG_L   s2,  a0, 0x28
  REG_L   s3,  a0, 0x30
  REG_L   s4,  a0, 0x38
  REG_L   s5,  a0, 0x40
  REG_L   s6,  a0, 0x48
  REG_L   s7,  a0, 0x50
  REG_L   s8,  a0, 0x58
  INT_ADDI a0, zero, 1    # a0 = 1
  beqz a1, .L0           # if (a1 == 0); goto L0
  move a0, a1           # a0 = a1
.L0:
  jr ra
END(_longjmp)
