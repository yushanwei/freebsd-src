/*	$OpenBSD: param.h,v 1.11 1998/08/30 22:05:35 millert Exp $ */

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department and Ralph Campbell.
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
 *
 *	from: Utah Hdr: machparam.h 1.11 89/08/14
 *	from: @(#)param.h	8.1 (Berkeley) 6/10/93
 *	JNPR: param.h,v 1.6.2.1 2007/09/10 07:49:36 girish
 */

#ifndef _MACHINE_PARAM_H_
#define	_MACHINE_PARAM_H_

#include <machine/_align.h>

#ifndef MACHINE
# define MACHINE	"loongarch"
#endif
#ifndef MACHINE_ARCH
# define MACHINE_ARCH 	"loongarch64"
#endif

/*
 * OBJFORMAT_NAMES is a comma-separated list of the object formats
 * that are supported on the architecture.
 */
#define	OBJFORMAT_NAMES		"elf"
#define	OBJFORMAT_DEFAULT	"elf"

#define	MID_MACHINE	0	/* None but has to be defined */

#ifdef SMP
#define	MAXSMPCPU	32
#ifndef MAXCPU
#define	MAXCPU		MAXSMPCPU
#endif
#else
#define	MAXSMPCPU	1
#define	MAXCPU		1
#endif

#ifndef MAXMEMDOM
#define	MAXMEMDOM	1
#endif

#define ALIGNBYTES      _ALIGNBYTES
#define ALIGN(p)        _ALIGN(p)
#define STACKALIGNBYTES (16 - 1)
#define STACKALIGN(p)   ((uint64_t)(p) & ~STACKALIGNBYTES)


/*
 * ALIGNED_POINTER is a boolean macro that checks whether an address
 * is valid to fetch data elements of type t from on this architecture.
 * This does not reflect the optimal alignment, just the possibility
 * (within reasonable limits). 
 */
#define	ALIGNED_POINTER(p, t)	((((unsigned long)(p)) & (sizeof (t) - 1)) == 0)

/*
 * CACHE_LINE_SIZE is the compile-time maximum cache line size for an
 * architecture.  It should be used with appropriate caution.
 */
#define	CACHE_LINE_SHIFT	6
#define	CACHE_LINE_SIZE		(1 << CACHE_LINE_SHIFT)

#define	PAGE_SHIFT		12		/* LOG2(PAGE_SIZE) */
#define	PAGE_SIZE		(1<<PAGE_SHIFT) /* bytes/page */
#define	PAGE_MASK		(PAGE_SIZE-1)

#define	NPTEPG			(PAGE_SIZE/(sizeof (pt_entry_t)))
#define	NPDEPG			(PAGE_SIZE/(sizeof (pd_entry_t)))

#define	NPTEPGSHIFT		9               /* LOG2(NPTEPG) */

#ifdef __loongarch64
#define	NPDEPGSHIFT		9               /* LOG2(NPTEPG) */
#define	SEGSHIFT		(PAGE_SHIFT + NPTEPGSHIFT + NPDEPGSHIFT)
#define	NBSEG			(1ul << SEGSHIFT)
#define	PDRSHIFT		(PAGE_SHIFT + NPTEPGSHIFT)
#define	PDRSIZE			(1ul << PDRSHIFT)
#define	PDRMASK			((1 << PDRSHIFT) - 1)
#else
#define	NPDEPGSHIFT		10               /* LOG2(NPTEPG) */
#define	SEGSHIFT		(PAGE_SHIFT + NPTEPGSHIFT)
#define	NBSEG			(1 << SEGSHIFT)	/* bytes/segment */
#define	PDRSHIFT		SEGSHIFT	/* alias for SEG in 32 bit */
#define	PDRSIZE			(1ul << PDRSHIFT)
#define	PDRMASK			((1 << PDRSHIFT) - 1)
#endif
#define	NBPDR			(1 << PDRSHIFT)	/* bytes/pagedir */
#define	SEGMASK			(NBSEG - 1)	/* byte offset into segment */

#define	MAXPAGESIZES		1		/* max supported pagesizes */

#define	MAXDUMPPGS		1		/* xxx: why is this only one? */

/*
 * The kernel stack needs to be aligned on a (PAGE_SIZE * 2) boundary.
 */
#define	KSTACK_PAGES		2	/* kernel stack */
#define	KSTACK_GUARD_PAGES	2	/* pages of kstack guard; 0 disables */

/*
 * Mach derived conversion macros
 */
#define	round_page(x)		(((x) + PAGE_MASK) & ~PAGE_MASK)
#define	trunc_page(x)		((x) & ~PAGE_MASK)

#define	atop(x)			((x) >> PAGE_SHIFT)
#define	ptoa(x)			((x) << PAGE_SHIFT)

#define	pgtok(x)		((x) * (PAGE_SIZE / 1024))

#endif /* !_MACHINE_PARAM_H_ */
