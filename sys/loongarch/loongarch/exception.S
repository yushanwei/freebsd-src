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

#include <machine/asm.h>
#include "assym.inc"

#include <machine/trap.h>
#include <machine/loongarchreg.h>

.macro save_registers mode

.if \mode == 0	/* We came from userspace. */
	csrrd	t0, LOONGARCH_CSR_KS0

	move	t1, sp
	move	sp, t0	/* switch to kernel sp */

	/* save user sp 
	ST	t1, sp, TF_R3
*/
.endif
	// save old stack pointer
	csrwr	sp, LOONGARCH_CSR_KS0
	csrrd	sp, LOONGARCH_CSR_KS0

	// push GP registers
	ADDI	sp, sp, -(GP_REG_CONTEXT_SIZE + FP_REG_CONTEXT_SIZE + CSR_REG_CONTEXT_SIZE)

	ST	zero, sp, 0 * SZREG
	ST	ra, sp, 1 * SZREG
	ST	tp, sp, 2 * SZREG
	ST	a0, sp, 4 * SZREG
	ST	a1, sp, 5 * SZREG
	ST	a2, sp, 6 * SZREG
	ST	a3, sp, 7 * SZREG
	ST	a4, sp, 8 * SZREG
	ST	a5, sp, 9 * SZREG
	ST	a6, sp, 10 * SZREG
	ST	a7, sp, 11 * SZREG

	ST	t0, sp, 12 * SZREG
	ST	t1, sp, 13 * SZREG
	ST	t2, sp, 14 * SZREG
	ST	t3, sp, 15 * SZREG
	ST	t4, sp, 16 * SZREG
	ST	t5, sp, 17 * SZREG
	ST	t6, sp, 18 * SZREG
	ST	t7, sp, 19 * SZREG
	ST	t8, sp, 20 * SZREG
	ST	u0, sp, 21 * SZREG
	ST	fp, sp, 22 * SZREG

	/* save static regiter */
	ST	s0, sp, 23 * SZREG
	ST	s1, sp, 24 * SZREG
	ST	s2, sp, 25 * SZREG
	ST	s3, sp, 26 * SZREG
	ST	s4, sp, 27 * SZREG
	ST	s5, sp, 28 * SZREG
	ST	s6, sp, 29 * SZREG
	ST	s7, sp, 30 * SZREG
	ST	s8, sp, 31 * SZREG
.if \mode == 0
	/* Store kernel sp */
	csrrd	t0, LOONGARCH_CSR_KS0	// read old sp
	ST	t0, sp, 3 * SZREG
.endif
	// push CSR registers
	ADDI	sp, sp, GP_REG_CONTEXT_SIZE

	csrrd	t0, LOONGARCH_CSR_CRMD
	ST	t0, sp, LOONGARCH_CSR_CRMD * SZREG
	csrrd	t0, LOONGARCH_CSR_PRMD
	ST	t0, sp, LOONGARCH_CSR_PRMD * SZREG
	csrrd	t0, LOONGARCH_CSR_EUEN
	ST	t0, sp, LOONGARCH_CSR_EUEN * SZREG
	csrrd	t0, LOONGARCH_CSR_MISC
	ST	t0, sp, LOONGARCH_CSR_MISC * SZREG
	csrrd	t0, LOONGARCH_CSR_ECFG
	ST	t0, sp, LOONGARCH_CSR_ECFG * SZREG
	csrrd	t0, LOONGARCH_CSR_ESTAT
	ST	t0, sp, LOONGARCH_CSR_ESTAT * SZREG
	csrrd	t0, LOONGARCH_CSR_ERA
	ST	t0, sp, LOONGARCH_CSR_ERA * SZREG
	csrrd	t0, LOONGARCH_CSR_BADV
	ST	t0, sp, LOONGARCH_CSR_BADV * SZREG
	csrrd	t0, LOONGARCH_CSR_BADI
	ST	t0, sp, LOONGARCH_CSR_BADI * SZREG

	// push FP registers
	ADDI	sp, sp, CSR_REG_CONTEXT_SIZE
	csrrd	t0, LOONGARCH_CSR_EUEN
	andi	t0, t0, CSR_EUEN_FPEN
	beqz	t0, PushRegDone_\@

	FST  fa0, sp, 0 * SZREG
	FST  fa1, sp, 1 * SZREG
	FST  fa2, sp, 2 * SZREG
	FST  fa3, sp, 3 * SZREG
	FST  fa4, sp, 4 * SZREG
	FST  fa5, sp, 5 * SZREG
	FST  fa6, sp, 6 * SZREG
	FST  fa7, sp, 7 * SZREG
	FST  ft0, sp, 8 * SZREG
	FST  ft1, sp, 9 * SZREG
	FST  ft2, sp, 10 * SZREG
	FST  ft3, sp, 11 * SZREG
	FST  ft4, sp, 12 * SZREG
	FST  ft5, sp, 13 * SZREG
	FST  ft6, sp, 14 * SZREG
	FST  ft7, sp, 15 * SZREG
	FST  ft8, sp, 16 * SZREG
	FST  ft9, sp, 17 * SZREG
	FST  ft10, sp, 18 * SZREG
	FST  ft11, sp, 19 * SZREG
	FST  ft12, sp, 20 * SZREG
	FST  ft13, sp, 21 * SZREG
	FST  ft14, sp, 22 * SZREG
	FST  ft15, sp, 23 * SZREG
	FST  fs0, sp, 24 * SZREG
	FST  fs1, sp, 25 * SZREG
	FST  fs2, sp, 26 * SZREG
	FST  fs3, sp, 27 * SZREG
	FST  fs4, sp, 28 * SZREG
	FST  fs5, sp, 29 * SZREG
	FST  fs6, sp, 30 * SZREG
	FST  fs7, sp, 31 * SZREG

	movfcsr2gr  t3, fcsr0
	ST        t3, sp, 32 * SZREG  // Push the FCSR0 register.

	// Push the fcc0-fcc7 registers.
	movcf2gr    t3, fcc0
	or          t2, t3, zero
	movcf2gr    t3, fcc1
	BSTRINS   t2, t3, 0xf, 0x8
	movcf2gr    t3, fcc2
	BSTRINS   t2, t3, 0x17, 0x10
	movcf2gr    t3, fcc3
	BSTRINS   t2, t3, 0x1f, 0x18
	movcf2gr    t3, fcc4
	BSTRINS   t2, t3, 0x27, 0x20
	movcf2gr    t3, fcc5
	BSTRINS   t2, t3, 0x2f, 0x28
	movcf2gr    t3, fcc6
	BSTRINS   t2, t3, 0x37, 0x30
	movcf2gr    t3, fcc7
	BSTRINS   t2, t3, 0x3f, 0x38
	ST        t2, sp, 33 * SZREG
	// Push exception context down
PushRegDone_\@:

	// point sp to stack top
	addi.d	sp, sp, -(GP_REG_CONTEXT_SIZE + CSR_REG_CONTEXT_SIZE)

.endm

.macro load_registers mode

	LD	t1, sp, (LOONGARCH_CSR_EUEN * SZREG + GP_REG_CONTEXT_SIZE)
	csrrd	t0, LOONGARCH_CSR_EUEN
	andi	t0, t0, CSR_EUEN_FPEN
	andi	t1, t1, CSR_EUEN_FPEN
	beq	t0, t1, PopRegs_\@
	beqz	t1, CloseFP_\@

CloseFP_\@:
	// FIXME
PopRegs_\@:

	// pop CSR registers
	addi.d  sp, sp, GP_REG_CONTEXT_SIZE

	LD    t0, sp, LOONGARCH_CSR_CRMD * SZREG
	csrwr   t0, LOONGARCH_CSR_CRMD
	LD    t0, sp, LOONGARCH_CSR_PRMD * SZREG
	csrwr   t0, LOONGARCH_CSR_PRMD
	LD    t0, sp, LOONGARCH_CSR_ECFG * SZREG
	csrwr   t0, LOONGARCH_CSR_ECFG
	LD    t0, sp, LOONGARCH_CSR_ERA * SZREG
	csrwr   t0, LOONGARCH_CSR_ERA

	addi.d  sp, sp, CSR_REG_CONTEXT_SIZE  // Fource change the stack pointer befor pop the FP registers.

	beqz    t1, PopGP_\@                      // If the FPE not set, only pop the GP registers.

	// Pop FP registers
	FLD  fa0, sp, 0 * SZREG
	FLD  fa1, sp, 1 * SZREG
	FLD  fa2, sp, 2 * SZREG
	FLD  fa3, sp, 3 * SZREG
	FLD  fa4, sp, 4 * SZREG
	FLD  fa5, sp, 5 * SZREG
	FLD  fa6, sp, 6 * SZREG
	FLD  fa7, sp, 7 * SZREG
	FLD  ft0, sp, 8 * SZREG
	FLD  ft1, sp, 9 * SZREG
	FLD  ft2, sp, 10 * SZREG
	FLD  ft3, sp, 11 * SZREG
	FLD  ft4, sp, 12 * SZREG
	FLD  ft5, sp, 13 * SZREG
	FLD  ft6, sp, 14 * SZREG
	FLD  ft7, sp, 15 * SZREG
	FLD  ft8, sp, 16 * SZREG
	FLD  ft9, sp, 17 * SZREG
	FLD  ft10, sp, 18 * SZREG
	FLD  ft11, sp, 19 * SZREG
	FLD  ft12, sp, 20 * SZREG
	FLD  ft13, sp, 21 * SZREG
	FLD  ft14, sp, 22 * SZREG
	FLD  ft15, sp, 23 * SZREG
	FLD  fs0, sp, 24 * SZREG
	FLD  fs1, sp, 25 * SZREG
	FLD  fs2, sp, 26 * SZREG
	FLD  fs3, sp, 27 * SZREG
	FLD  fs4, sp, 28 * SZREG
	FLD  fs5, sp, 29 * SZREG
	FLD  fs6, sp, 30 * SZREG
	FLD  fs7, sp, 31 * SZREG

	LD        t0, sp, 32 * SZREG
	movgr2fcsr  fcsr0, t0	// pop fcsr0 register

	// Pop the fcc0-fcc7 registers.
	LD        t0, sp, 33 * SZREG
	bstrpick.d  t1, t0, 7, 0
	movgr2cf    fcc0, t1
	bstrpick.d  t1, t0, 15, 8
	movgr2cf    fcc1, t1
	bstrpick.d  t1, t0, 23, 16
	movgr2cf    fcc2, t1
	bstrpick.d  t1, t0, 31, 24
	movgr2cf    fcc3, t1
	bstrpick.d  t1, t0, 39, 32
	movgr2cf    fcc4, t1
	bstrpick.d  t1, t0, 47, 40
	movgr2cf    fcc5, t1
	bstrpick.d  t1, t0, 55, 48
	movgr2cf    fcc6, t1
	bstrpick.d  t1, t0, 63, 56
	movgr2cf    fcc7, t1

PopGP_\@:
	// pop GP registers
	addi.d  sp, sp, -(GP_REG_CONTEXT_SIZE + CSR_REG_CONTEXT_SIZE)
	LD    ra, sp, 1 * SZREG
	LD    tp, sp, 2 * SZREG
	LD    a0, sp, 4 * SZREG
	LD    a1, sp, 5 * SZREG
	LD    a2, sp, 6 * SZREG
	LD    a3, sp, 7 * SZREG
	LD    a4, sp, 8 * SZREG
	LD    a5, sp, 9 * SZREG
	LD    a6, sp, 10 * SZREG
	LD    a7, sp, 11 * SZREG
	LD    t0, sp, 12 * SZREG
	LD    t1, sp, 13 * SZREG
	LD    t2, sp, 14 * SZREG
	LD    t3, sp, 15 * SZREG
	LD    t4, sp, 16 * SZREG
	LD    t5, sp, 17 * SZREG
	LD    t6, sp, 18 * SZREG
	LD    t7, sp, 19 * SZREG
	LD    t8, sp, 20 * SZREG
	LD    u0, sp, 21 * SZREG
	LD    fp, sp, 22 * SZREG
	LD    s0, sp, 23 * SZREG
	LD    s1, sp, 24 * SZREG
	LD    s2, sp, 25 * SZREG
	LD    s3, sp, 26 * SZREG
	LD    s4, sp, 27 * SZREG
	LD    s5, sp, 28 * SZREG
	LD    s6, sp, 29 * SZREG
	LD    s7, sp, 30 * SZREG
	LD    s8, sp, 31 * SZREG

.if \mode == 0
/* We go to userspace. Load user sp
	LD	sp, sp, TF_R3
 */

.endif
.endm

.macro	do_ast
	/* Disable interrupts */
	
1:
	

	
	

	beqz	a2, 2f

	/* Restore interrupts */

	

	/* Handle the ast */

	call36	_C_LABEL(ast)

	/* Re-check for new ast scheduled */
	b	1b
2:
.endm

ENTRY(cpu_exception_handler)
	csrrd	t0, LOONGARCH_CSR_PRMD
	andi	t0, t0, 0x3
	beqz	t0, 1f
	/* User mode detected */
	b	cpu_exception_handler_user
1:
	/* Supervisor mode detected */
	b	cpu_exception_handler_supervisor
END(cpu_exception_handler)

ENTRY(cpu_exception_handler_supervisor)
	save_registers 1
	move	a0, sp
	call36	_C_LABEL(do_trap_supervisor)
	load_registers 1
	ertn
END(cpu_exception_handler_supervisor)

ENTRY(cpu_exception_handler_user)
	save_registers 0
	move	a0, sp
	call36	_C_LABEL(do_trap_user)
	do_ast
	load_registers 0
	ertn
END(cpu_exception_handler_user)
