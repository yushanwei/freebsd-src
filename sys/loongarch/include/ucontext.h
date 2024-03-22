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

#ifndef _MACHINE_UCONTEXT_H_
#define	_MACHINE_UCONTEXT_H_

struct gpregs {
	__uint64_t	gp_ra;
	__uint64_t	gp_sp;
	__uint64_t	gp_fp;
	__uint64_t	gp_tp;
	__uint64_t	gp_a[8];
	__uint64_t	gp_t[10];
	__uint64_t	gp_s[9];
	__uint64_t	gp_sepc;
	__uint64_t	gp_sstatus;
};

struct fpregs {
	__uint64_t	fp_x[32][2];
	__uint64_t	fp_fcsr;
	int		fp_flags;
	int		pad;
};

typedef struct {
	int		mc_flags;
	struct gpregs	mc_gpregs;
	struct fpregs	mc_fpregs;
#define	_MC_FP_VALID	0x1		/* Set when mc_fpregs has valid data */
	int		mc_pad;
	__uint64_t	mc_spare[8];	/* Space for expansion */
}mcontext_t;

/* Userlevel context.
typedef struct {
	int		uc_flags;
	struct ucontext_t *uc_link;
	struct gpregs	mc_gpregs;
	struct fpregs	mc_fpregs;
  mcontext_t uc_mcontext;
}ucontext_t;
  unsigned long int __uc_flags;
  struct ucontext_t *uc_link;
  stack_t uc_stack;
  sigset_t uc_sigmask;

 Structure describing a signal stack.
typedef struct
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;

#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
  unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

*/

#endif	/* !_MACHINE_UCONTEXT_H_ */
