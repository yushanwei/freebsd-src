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

#ifndef	_MACHINE_REG_H_
#define	_MACHINE_REG_H_

#include <sys/_types.h>

struct reg {
	__uint64_t	ra;		/* return address */
	__uint64_t	tp;		/* thread pointer */
	__uint64_t	sp;		/* stack pointer */
	__uint64_t	a[8];		/* function arguments */
	__uint64_t	t[9];		/* temporaries */
	__uint64_t	u0;
	__uint64_t	fp;
	__uint64_t	s[9];		/* saved registers */
/* Special CSR register */
	__uint64_t	crmd;
	__uint64_t	prmd;
	__uint64_t	euen;
	__uint64_t	misc;
	__uint64_t	ecfg;
	__uint64_t	estat;
	__uint64_t	era;
	__uint64_t	badv;
};

struct fpreg {
	__uint64_t      fp_a[8];	/* Floating point registers */
	__uint64_t      fp_t[16];	/* Floating point registers */
	__uint64_t      fp_s[8];	/* Floating point registers */
	__uint64_t      fp_fcc;		/* Floating point CFR */
	int      	fp_fcsr;	/* Floating point control reg */
	int		t;
};

struct dbreg {
	int dummy;
};

#endif /* !_MACHINE_REG_H_ */
