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

#include <sys/param.h>

#include <machine/loongarchreg.h>

#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ucontext.h>

void _ctx_start(void);

void
ctx_done(ucontext_t *ucp)
{

	if (ucp->uc_link == NULL) {
		exit(0);
	} else {
		setcontext((const ucontext_t *)ucp->uc_link);
		abort();
	}
}

__weak_reference(__makecontext, makecontext);

void
__makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	struct reg *gp;
	va_list ap;
	int i;

	/* A valid context is required. */
	if (ucp == NULL)
		return;

	if ((argc < 0) || (argc > 8))
		return;

	gp = &ucp->uc_mcontext.mc_regs;

	va_start(ap, argc);
	/* Pass up to eight arguments in a0-7. */
	for (i = 0; i < argc && i < 8; i++)
		gp->a[i] = va_arg(ap, uint64_t);
	va_end(ap);

	/* Set the stack */
	gp->sp = STACKALIGN(ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size);
	/* Arrange for return via the trampoline code. */
	gp->era = (__register_t)_ctx_start;
	gp->s[0] = (__register_t)func;
	gp->s[1] = (__register_t)ucp;
}
