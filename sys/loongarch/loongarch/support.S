/*-
 * Copyright (c) 2015-2020 Ruslan Bukin <br@bsdpad.com>
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
#include <machine/loongarchreg.h>

#include "assym.inc"

/*
 * One of the fu* or su* functions failed, return -1.
 */
ENTRY(fsu_fault)
	SET_FAULT_HANDLER(zero, a1)	/* Reset the handler function */
	EXIT_USER_ACCESS(a1)
fsu_fault_nopcb:
	LDI	a0, -1
	ret
END(fsu_fault)

/*
 * int casueword32(volatile uint32_t *, uint32_t, uint32_t *, uint32_t)
 */
ENTRY(casueword32)
	LDI	a4, (VM_MAXUSER_ADDRESS-3)
	bgeu	a0, a4, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a4)	/* And set it */
	ENTER_USER_ACCESS(a4)
	LL	a4, a0, 0		/* Load-exclusive the data */
	bne	a4, a1, 1f		/* If not equal then exit */
	SC	a3, a0, 0		/* Store the new data */
	beqz	a5, 2f			/* Success */
1:	LDI	a5, 1			/* Normalize failure result */
2:	EXIT_USER_ACCESS(a6)
	SET_FAULT_HANDLER(zero, a6)	/* Reset the fault handler */
	ST	a4, a2,	0	/* Store the read data */
	move	a0, a5			/* Success indicator */
	ret				/* Return */
END(casueword32)

/*
 * int casueword(volatile u_long *, u_long, u_long *, u_long)
 */
ENTRY(casueword)
	LDI	a4, (VM_MAXUSER_ADDRESS-7)
	bgeu	a0, a4, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a4)	/* And set it */
	ENTER_USER_ACCESS(a4)
	LL	a4, a0, 0		/* Load-exclusive the data */
	bne	a4, a1, 1f		/* If not equal then exit */
	SC	a3, a0, 0		/* Store the new data */
	beqz	a5, 2f			/* Success */
1:	LDI	a5, 1			/* Normalize failure result */
2:	EXIT_USER_ACCESS(a6)
	SET_FAULT_HANDLER(zero, a6)	/* Reset the fault handler */
	ST	a4, a2, 0		/* Store the read data */
	move	a0, a5			/* Success indicator */
	ret				/* Return */
END(casueword)

/*
 * int fubyte(volatile const void *)
 */
ENTRY(fubyte)
	LDI	a1, VM_MAXUSER_ADDRESS
	bgeu	a0, a1, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a1)	/* And set it */
	ENTER_USER_ACCESS(a1)
	ld.bu	a0, a0, 0		/* Try loading the data */
	EXIT_USER_ACCESS(a1)
	SET_FAULT_HANDLER(zero, a1)	/* Reset the fault handler */
	ret				/* Return */
END(fubyte)

/*
 * int fuword(volatile const void *)
 */
ENTRY(fuword16)
	LDI	a1, (VM_MAXUSER_ADDRESS-1)
	bgeu	a0, a1, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a1)	/* And set it */
	ENTER_USER_ACCESS(a1)
	ld.hu	a0, a0, 0		/* Try loading the data */
	EXIT_USER_ACCESS(a1)
	SET_FAULT_HANDLER(zero, a1)	/* Reset the fault handler */
	ret				/* Return */
END(fuword16)

/*
 * int32_t fueword32(volatile const void *, int32_t *)
 */
ENTRY(fueword32)
	LDI	a2, (VM_MAXUSER_ADDRESS-3)
	bgeu	a0, a2, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a2)	/* And set it */
	ENTER_USER_ACCESS(a2)
	LD	a0, a0, 0		/* Try loading the data */
	EXIT_USER_ACCESS(a2)
	SET_FAULT_HANDLER(zero, a2)	/* Reset the fault handler */
	ST	a0, a1, 0		/* Save the data in kernel space */
	LDI	a0, 0			/* Success */
	ret				/* Return */
END(fueword32)

/*
 * long fueword(volatile const void *, int64_t *)
 * int64_t fueword64(volatile const void *, int64_t *)
 */
ENTRY(fueword)
EENTRY(fueword64)
	LDI	a2, (VM_MAXUSER_ADDRESS-7)
	bgeu	a0, a2, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a2)	/* And set it */
	ENTER_USER_ACCESS(a2)
	LD	a0, a0, 0		/* Try loading the data */
	EXIT_USER_ACCESS(a2)
	SET_FAULT_HANDLER(zero, a2)	/* Reset the fault handler */
	ST	a0, a1, 0		/* Save the data in kernel space */
	LDI	a0, 0			/* Success */
	ret				/* Return */
EEND(fueword64)
END(fueword)

/*
 * int subyte(volatile void *, int)
 */
ENTRY(subyte)
	LDI	a2, VM_MAXUSER_ADDRESS
	bgeu	a0, a2, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a2)	/* And set it */
	ENTER_USER_ACCESS(a2)
	st.b	a1, a0, 0		/* Try storing the data */
	EXIT_USER_ACCESS(a2)
	SET_FAULT_HANDLER(zero, a2)	/* Reset the fault handler */
	LDI	a0, 0			/* Success */
	ret				/* Return */
END(subyte)

/*
 * int suword16(volatile void *, int)
 */
ENTRY(suword16)
	LDI	a2, (VM_MAXUSER_ADDRESS-1)
	bgeu	a0, a2, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a2)	/* And set it */
	ENTER_USER_ACCESS(a2)
	st.h	a1, a0, 0		/* Try storing the data */
	EXIT_USER_ACCESS(a2)
	SET_FAULT_HANDLER(zero, a2)	/* Reset the fault handler */
	LDI	a0, 0			/* Success */
	ret				/* Return */
END(suword16)

/*
 * int suword32(volatile void *, int)
 */
ENTRY(suword32)
	LDI	a2, (VM_MAXUSER_ADDRESS-3)
	bgeu	a0, a2, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a2)	/* And set it */
	ENTER_USER_ACCESS(a2)
	st.w	a1, a0, 0		/* Try storing the data */
	EXIT_USER_ACCESS(a2)
	SET_FAULT_HANDLER(zero, a2)	/* Reset the fault handler */
	LDI	a0, 0			/* Success */
	ret				/* Return */
END(suword32)

/*
 * int suword(volatile void *, long)
 */
ENTRY(suword)
EENTRY(suword64)
	LDI	a2, (VM_MAXUSER_ADDRESS-7)
	bgeu	a0, a2, fsu_fault_nopcb
	la	a6, fsu_fault		/* Load the fault handler */
	SET_FAULT_HANDLER(a6, a2)	/* And set it */
	ENTER_USER_ACCESS(a2)
	st.d	a1, a0, 0		/* Try storing the data */
	EXIT_USER_ACCESS(a2)
	SET_FAULT_HANDLER(zero, a2)	/* Reset the fault handler */
	LDI	a0, 0			/* Success */
	ret				/* Return */
EEND(suword64)
END(suword)

ENTRY(setjmp)
	/* Store the stack pointer */
	ST	sp, a0, 0
	ADDI	a0, a0, SZREG

	/* Store the general purpose registers and ra */
	ST	s0, a0, (0 * SZREG)
	ST	s1, a0, (1 * SZREG)
	ST	s2, a0, (2 * SZREG)
	ST	s3, a0, (3 * SZREG)
	ST	s4, a0, (4 * SZREG)
	ST	s5, a0, (5 * SZREG)
	ST	s6, a0, (6 * SZREG)
	ST	s7, a0, (7 * SZREG)
	ST	s8, a0, (8 * SZREG)
	ST	s9, a0, (9 * SZREG)
	ST	ra, a0, (12 * SZREG)

	/* Return value */
	LDI	a0, 0
	ret
END(setjmp)

ENTRY(longjmp)
	/* Restore the stack pointer */
	LD	sp, a0, 0
	ADDI	a0, a0, SZREG

	/* Restore the general purpose registers and ra */
	LD	s0, a0, (0 * SZREG)
	LD	s1, a0, (1 * SZREG)
	LD	s2, a0, (2 * SZREG)
	LD	s3, a0, (3 * SZREG)
	LD	s4, a0, (4 * SZREG)
	LD	s5, a0, (5 * SZREG)
	LD	s6, a0, (6 * SZREG)
	LD	s7, a0, (7 * SZREG)
	LD	s8, a0, (8 * SZREG)
	LD	s9, a0, (9 * SZREG)
	LD	ra, a0, (12 * SZREG)

	/* Load the return value */
	move	a0, a1
	ret
END(longjmp)
