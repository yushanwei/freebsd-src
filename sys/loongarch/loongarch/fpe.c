/*-
 * Copyright (c) 2024 Ruslan Bukin <br@bsdpad.com>
 *
 * This software was developed by the University of Cambridge Computer
 * Laboratory (Department of Computer Science and Technology) under Innovate
 * UK project 105694, "Digital Security by Design (DSbD) Technology Platform
 * Prototype".
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
#include <sys/systm.h>
#include <sys/kernel.h>

#include <vm/uma.h>

#include <machine/fpe.h>
#include <machine/reg.h>
#include <machine/loongarchreg.h>

static uma_zone_t fpu_save_area_zone;
static struct fpreg *fpu_initialstate;

inline void fpe_enable(void) 
{
        set_csr_euen(CSR_EUEN_FPEN);
}
inline void fpe_disable(void) 
{
        clear_csr_euen(CSR_EUEN_FPEN);
}

void
fpe_store(struct fpreg *regs)
{
	uint64_t fcsr, fcc, (*fp_x)[32][2];

	fcsr = regs->fp_fcsr;
	fcc = regs->fp_fcc;
	fp_x = &regs->fp_x;

/*save_fp*/
	__asm __volatile(
	    "fst.d	$f0, %0, (16 * 0)\n"
	    "fst.d	$f1, %0, (16 * 1)\n"
	    "fst.d	$f2, %0, (16 * 2)\n"
	    "fst.d	$f3, %0, (16 * 3)\n"
	    "fst.d	$f4, %0, (16 * 4)\n"
	    "fst.d	$f5, %0, (16 * 5)\n"
	    "fst.d	$f6, %0, (16 * 6)\n"
	    "fst.d	$f7, %0, (16 * 7)\n"
	    "fst.d	$f8, %0, (16 * 8)\n"
	    "fst.d	$f9, %0, (16 * 9)\n"
	    "fst.d	$f10, %0, (16 * 10)\n"
	    "fst.d	$f11, %0, (16 * 11)\n"
	    "fst.d	$f12, %0, (16 * 12)\n"
	    "fst.d	$f13, %0, (16 * 13)\n"
	    "fst.d	$f14, %0, (16 * 14)\n"
	    "fst.d	$f15, %0, (16 * 15)\n"
	    "fst.d	$f16, %0, (16 * 16)\n"
	    "fst.d	$f17, %0, (16 * 17)\n"
	    "fst.d	$f18, %0, (16 * 18)\n"
	    "fst.d	$f19, %0, (16 * 19)\n"
	    "fst.d	$f20, %0, (16 * 20)\n"
	    "fst.d	$f21, %0, (16 * 21)\n"
	    "fst.d	$f22, %0, (16 * 22)\n"
	    "fst.d	$f23, %0, (16 * 23)\n"
	    "fst.d	$f24, %0, (16 * 24)\n"
	    "fst.d	$f25, %0, (16 * 25)\n"
	    "fst.d	$f26, %0, (16 * 26)\n"
	    "fst.d	$f27, %0, (16 * 27)\n"
	    "fst.d	$f28, %0, (16 * 28)\n"
	    "fst.d	$f29, %0, (16 * 29)\n"
	    "fst.d	$f30, %0, (16 * 30)\n"
	    "fst.d	$f31, %0, (16 * 31)\n"
	    : "=r"(fp_x), "=m"(*fp_x));

/*save_fcc*/
	__asm __volatile(
	    "movcf2gr	$t1, $fcc0\n"
	    "move	$t2, $t1\n"
	    "movcf2gr	$t1, $fcc1\n"
	    "bstrins.d	$t2, $t1, 15, 8\n"
	    "movcf2gr	$t1, $fcc2\n"
	    "bstrins.d	$t2, $t1, 23, 16\n"
	    "movcf2gr	$t1, $fcc3\n"
	    "bstrins.d	$t2, $t1, 31, 24\n"
	    "movcf2gr	$t1, $fcc4\n"
	    "bstrins.d	$t2, $t1, 39, 32\n"
	    "movcf2gr	$t1, $fcc5\n"
	    "bstrins.d	$t2, $t1, 47, 40\n"
	    "movcf2gr	$t1, $fcc6\n"
	    "bstrins.d	$t2, $t1, 55, 48\n"
	    "movcf2gr	$t1, $fcc7\n"
	    "bstrins.d	$t2, $t1, 63, 56\n"
	    "st.d	$t2, %0, 0\n"
	    :: "r"(fcc));

/*save_fcsr*/
	regs->fp_fcsr = fcsr;
}

void
fpe_restore(struct fpreg *regs)
{
	uint64_t fcsr, fcc, (*fp_x)[32][2];

	fp_x = &regs->fp_x;
	fcsr = regs->fp_fcsr;
	fcc  = regs->fp_fcc;

/*restore_fp*/
	__asm __volatile(
	    "fld.d	$f0, %0, (16 * 0)\n"
	    "fld.d	$f1, %0, (16 * 1)\n"
	    "fld.d	$f2, %0, (16 * 2)\n"
	    "fld.d	$f3, %0, (16 * 3)\n"
	    "fld.d	$f4, %0, (16 * 4)\n"
	    "fld.d	$f5, %0, (16 * 5)\n"
	    "fld.d	$f6, %0, (16 * 6)\n"
	    "fld.d	$f7, %0, (16 * 7)\n"
	    "fld.d	$f8, %0, (16 * 8)\n"
	    "fld.d	$f9, %0, (16 * 9)\n"
	    "fld.d	$f10, %0, (16 * 10)\n"
	    "fld.d	$f11, %0, (16 * 11)\n"
	    "fld.d	$f12, %0, (16 * 12)\n"
	    "fld.d	$f13, %0, (16 * 13)\n"
	    "fld.d	$f14, %0, (16 * 14)\n"
	    "fld.d	$f15, %0, (16 * 15)\n"
	    "fld.d	$f16, %0, (16 * 16)\n"
	    "fld.d	$f17, %0, (16 * 17)\n"
	    "fld.d	$f18, %0, (16 * 18)\n"
	    "fld.d	$f19, %0, (16 * 19)\n"
	    "fld.d	$f20, %0, (16 * 20)\n"
	    "fld.d	$f21, %0, (16 * 21)\n"
	    "fld.d	$f22, %0, (16 * 22)\n"
	    "fld.d	$f23, %0, (16 * 23)\n"
	    "fld.d	$f24, %0, (16 * 24)\n"
	    "fld.d	$f25, %0, (16 * 25)\n"
	    "fld.d	$f26, %0, (16 * 26)\n"
	    "fld.d	$f27, %0, (16 * 27)\n"
	    "fld.d	$f28, %0, (16 * 28)\n"
	    "fld.d	$f29, %0, (16 * 29)\n"
	    "fld.d	$f30, %0, (16 * 30)\n"
	    "fld.d	$f31, %0, (16 * 31)\n"
	    :: "r"(fp_x), "m"(*fp_x));

/*restore_fcc*/
	__asm __volatile(
	    "ld.d	$t1, %0, 0\n"
	    "bstrpick.d	$t2, $t1, 7, 0\n"
	    "movgr2cf	$fcc0, $t2\n"
	    "bstrpick.d	$t2, $t1, 15, 8\n"
	    "movgr2cf	$fcc1, $t2\n"
	    "bstrpick.d	$t2, $t1, 23, 16\n"
	    "movgr2cf	$fcc2, $t2\n"
	    "bstrpick.d	$t2, $t1, 31, 24\n"
	    "movgr2cf	$fcc3, $t2\n"
	    "bstrpick.d	$t2, $t1, 39, 32\n"
	    "movgr2cf	$fcc4, $t2\n"
	    "bstrpick.d	$t2, $t1, 47, 40\n"
	    "movgr2cf	$fcc5, $t2\n"
	    "bstrpick.d	$t2, $t1, 55, 48\n"
	    "movgr2cf	$fcc6, $t2\n"
	    "bstrpick.d	$t2, $t1, 63, 56\n"
	    "movgr2cf	$fcc7, $t2\n"
	    :: "r"(fcc));

	regs->fp_fcsr = fcsr;
}

struct fpreg *
fpu_save_area_alloc(void)
{

	return (uma_zalloc(fpu_save_area_zone, M_WAITOK));
}

void
fpu_save_area_free(struct fpreg *fsa)
{

	uma_zfree(fpu_save_area_zone, fsa);
}

void
fpu_save_area_reset(struct fpreg *fsa)
{

	memcpy(fsa, fpu_initialstate, sizeof(*fsa));
}

static void
fpe_init(const void *dummy __unused)
{

	fpu_save_area_zone = uma_zcreate("FPE save area", sizeof(struct fpreg),
	    NULL, NULL, NULL, NULL, _Alignof(struct fpreg) - 1, 0);
	fpu_initialstate = uma_zalloc(fpu_save_area_zone, M_WAITOK | M_ZERO);

	fpe_enable();
	fpe_store(fpu_initialstate);
	fpe_disable();

	bzero(fpu_initialstate->fp_x, sizeof(fpu_initialstate->fp_x));
}

SYSINIT(fpe, SI_SUB_CPU, SI_ORDER_ANY, fpe_init, NULL);
