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

#ifndef _MACHINE_CPUFUNC_H_
#define	_MACHINE_CPUFUNC_H_

static __inline void
breakpoint(void)
{

	__asm("break 0");
}

static __inline u_long
rdtime(void)
{
	u_long val;
	__asm __volatile(
		"rdtime.d %0,$zero"
		: "=r" (val) :
	);
	return(val);
}

#ifdef _KERNEL

#include <sys/_null.h>

#include <machine/loongarchreg.h>

static __inline register_t
intr_disable(void)
{
	uint64_t flags;

	__asm __volatile(
		"csrxchg %[val], %[mask], %[reg]"
		: [val] "+r" (flags)
		: [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
		: "memory"
	);
	return (flags);
}

static __inline void
intr_restore(register_t s)
{
	uint32_t flags = s;
 
	__asm __volatile(
    		"csrxchg %[val], %[mask], %[reg]\n\t"
    		: [val] "+r" (flags)
    		: [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
    		: "memory");
}

static __inline void
intr_enable(void)
{
	uint64_t flags = CSR_CRMD_IE;
	__asm __volatile(
		"csrxchg %[val], %[mask], %[reg]"
		: [val] "+r" (flags)
		: [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
		: "memory");
}

/* NB: fence() is defined as a macro in <machine/atomic.h>. */

static __inline void
fence_i(void)
{

	__asm __volatile("ibar 0" ::);
}

#define	rdcycle()			csr_read64(cycle)
#define	rdtime()			csr_read64(time)
#define	rdinstret()			csr_read64(instret)
#define	rdhpmcounter(n)			csr_read64(hpmcounter##n)

/* Cache hooks. */

extern int64_t dcache_line_size;

typedef void (*cache_op_t)(vm_offset_t start, vm_size_t size);

struct loongarch_cache_ops {
	cache_op_t dcache_wbinv_range;
	cache_op_t dcache_inv_range;
	cache_op_t dcache_wb_range;
};

extern struct loongarch_cache_ops cache_ops;

static __inline void
cpu_dcache_wbinv_range(vm_offset_t addr, vm_size_t size)
{
	if (cache_ops.dcache_wbinv_range != NULL)
		cache_ops.dcache_wbinv_range(addr, size);
}

static __inline void
cpu_dcache_inv_range(vm_offset_t addr, vm_size_t size)
{
	if (cache_ops.dcache_inv_range != NULL)
		cache_ops.dcache_inv_range(addr, size);
}

static __inline void
cpu_dcache_wb_range(vm_offset_t addr, vm_size_t size)
{
	if (cache_ops.dcache_wb_range != NULL)
		cache_ops.dcache_wb_range(addr, size);
}

void loongarch_cache_install_hooks(struct loongarch_cache_ops *, u_int);

#define	cpufunc_nullop()		loongarch_nullop()

void loongarch_nullop(void);

#endif	/* _KERNEL */
#endif	/* _MACHINE_CPUFUNC_H_ */
