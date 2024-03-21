/*-
 * Copyright (c) 1986, 1989, 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *	from: FreeBSD: src/sys/i386/include/signal.h,v 1.13 2000/11/09
 *	from: FreeBSD: src/sys/sparc64/include/signal.h,v 1.6 2001/09/30 18:52:17
 */

#ifndef	_MACHINE_SIGNAL_H_
#define	_MACHINE_SIGNAL_H_

#include <sys/cdefs.h>

typedef	long sig_atomic_t;

#if __BSD_VISIBLE

/* FP context was used */
#define SC_USED_FP              (1 << 0)
/* Address error was due to memory load */
#define SC_ADDRERR_RD           (1 << 30)
/* Address error was due to memory store */
#define SC_ADDRERR_WR           (1 << 31)

struct sigcontext {
        __uint64_t   sc_pc;
        __uint64_t   sc_regs[32];
        __uint32_t   sc_flags;
        __uint64_t   sc_extcontext[0];
};

#define CONTEXT_INFO_ALIGN      16
struct sctx_info {
        __uint32_t   magic;
        __uint32_t   size;
        __uint64_t   padding;        /* padding to 16 bytes */
};
 
/* FPU context */
#define FPU_CTX_MAGIC           0x46505501
#define FPU_CTX_ALIGN           8
struct fpu_context {
        __uint64_t   regs[32];
        __uint64_t   fcc;
        __uint32_t   fcsr;
};
 
/* LSX context */
#define LSX_CTX_MAGIC           0x53580001
#define LSX_CTX_ALIGN           16
struct lsx_context {
        __uint64_t   regs[2*32];
        __uint64_t   fcc;
        __uint32_t   fcsr;
};

/* LASX context */
#define LASX_CTX_MAGIC          0x41535801
#define LASX_CTX_ALIGN          32
struct lasx_context {
        __uint64_t   regs[4*32];
        __uint64_t   fcc;
        __uint32_t   fcsr;
};

/* LBT context */
#define LBT_CTX_MAGIC           0x42540001
#define LBT_CTX_ALIGN           8
struct lbt_context {
        __uint64_t   regs[4];
        __uint32_t   eflags;
        __uint32_t   ftop;
};

#endif

#endif /* !_MACHINE_SIGNAL_H_ */
