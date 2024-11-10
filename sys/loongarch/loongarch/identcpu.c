/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2015-2016 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 * Copyright (c) 2022 Mitchell Horne <mhorne@FreeBSD.org>
 * Copyright (c) 2023 The FreeBSD Foundation
 *
 * Portions of this software were developed by SRI International and the
 * University of Cambridge Computer Laboratory under DARPA/AFRL contract
 * FA8750-10-C-0237 ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Portions of this software were developed by the University of Cambridge
 * Computer Laboratory as part of the CTSRD Project, with support from the
 * UK Higher Education Innovation Fund (HEIF).
 *
 * Portions of this software were developed by Mitchell Horne
 * <mhorne@FreeBSD.org> under sponsorship from the FreeBSD Foundation.
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

#include "opt_platform.h"

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ctype.h>
#include <sys/kernel.h>
#include <sys/pcpu.h>
#include <sys/sysctl.h>

#include <machine/cpu.h>
#include <machine/cpufunc.h>
#include <machine/elf.h>
#include <machine/md_var.h>

#ifdef FDT
#include <dev/fdt/fdt_common.h>
#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofw_bus_subr.h>
#endif

const char machine[] = "loongarch";

SYSCTL_CONST_STRING(_hw, HW_MACHINE, machine, CTLFLAG_RD | CTLFLAG_CAPRD,
    machine, "Machine class");

/* Hardware implementation info. These values may be empty. */
register_t mvendorid;	/* The CPU's JEDEC vendor ID */
register_t marchid;	/* The architecture ID */
register_t mimpid;	/* The implementation ID */

/* Supervisor-mode extension support. */
bool __read_frequently has_sstc;
bool __read_frequently has_sscofpmf;

struct cpu_desc {
	const char	*cpu_mvendor_name;
	const char	*cpu_march_name;

	struct cpuinfo_loongarch cpuinfo;
};

struct cpu_desc cpu_desc[MAXCPU];

/*
 * Micro-architecture tables.
 */
struct marchid_entry {
	register_t	march_id;
	const char	*march_name;
};

#define MARCHID_LA32 CPUCFG1_ISGR32
#define MARCHID_LA64 CPUCFG1_ISGR64
#define	MARCHID_END	{ -1ul, NULL }

#define MVENDORID_UNIMPL 0
#define MVENDORID_LOONGSON PRID_COMP_LOONGSON


static const struct marchid_entry loongson_marchids[] = {
	{ MARCHID_LA32, "Loongson-32bit" },
	{ MARCHID_LA64, "Loongson-64bit" },
	MARCHID_END,
};

/*
 * Known CPU vendor/manufacturer table.
 */
static const struct {
	register_t			mvendor_id;
	const char			*mvendor_name;
	const struct marchid_entry	*marchid_table;
} mvendor_ids[] = {
	{ MVENDORID_LOONGSON, "Loongson", loongson_marchids },
	{ MVENDORID_UNIMPL,	"Unspecified",		NULL		},
};

static void cpu_probe_common(struct cpuinfo_loongarch *c)
{
	unsigned int config;
	unsigned long asid_mask;

	c->options = LOONGARCH_CPU_CPUCFG | LOONGARCH_CPU_CSR |
		LOONGARCH_CPU_TLB | LOONGARCH_CPU_VINT | LOONGARCH_CPU_WATCH;

	c->hwcap = HWCAP_LOONGARCH_CPUCFG;

	config = read_cpucfg(LOONGARCH_CPUCFG1);
	if (config & CPUCFG1_UAL) {
		c->options |= LOONGARCH_CPU_UAL;
		c->hwcap |= HWCAP_LOONGARCH_UAL;
	}
	if (config & CPUCFG1_CRC32) {
		c->options |= LOONGARCH_CPU_CRC32;
		c->hwcap |= HWCAP_LOONGARCH_CRC32;
	}

	config = read_cpucfg(LOONGARCH_CPUCFG2);
	if (config & CPUCFG2_LAM) {
		c->options |= LOONGARCH_CPU_LAM;
		c->hwcap |= HWCAP_LOONGARCH_LAM;
	}
	if (config & CPUCFG2_FP) {
		c->options |= LOONGARCH_CPU_FPU;
		c->hwcap |= HWCAP_LOONGARCH_FPU;
	}
#ifdef CONFIG_CPU_HAS_LSX
	if (config & CPUCFG2_LSX) {
		c->options |= LOONGARCH_CPU_LSX;
		c->hwcap |= HWCAP_LOONGARCH_LSX;
	}
#endif
#ifdef CONFIG_CPU_HAS_LASX
	if (config & CPUCFG2_LASX) {
		c->options |= LOONGARCH_CPU_LASX;
		c->hwcap |= HWCAP_LOONGARCH_LASX;
	}
#endif
	if (config & CPUCFG2_COMPLEX) {
		c->options |= LOONGARCH_CPU_COMPLEX;
		c->hwcap |= HWCAP_LOONGARCH_COMPLEX;
	}
	if (config & CPUCFG2_CRYPTO) {
		c->options |= LOONGARCH_CPU_CRYPTO;
		c->hwcap |= HWCAP_LOONGARCH_CRYPTO;
	}
	if (config & CPUCFG2_PTW) {
		c->options |= LOONGARCH_CPU_PTW;
		c->hwcap |= HWCAP_LOONGARCH_PTW;
	}
	if (config & CPUCFG2_LVZP) {
		c->options |= LOONGARCH_CPU_LVZ;
		c->hwcap |= HWCAP_LOONGARCH_LVZ;
	}
#ifdef CONFIG_CPU_HAS_LBT
	if (config & CPUCFG2_X86BT) {
		c->options |= LOONGARCH_CPU_LBT_X86;
		c->hwcap |= HWCAP_LOONGARCH_LBT_X86;
	}
	if (config & CPUCFG2_ARMBT) {
		c->options |= LOONGARCH_CPU_LBT_ARM;
		c->hwcap |= HWCAP_LOONGARCH_LBT_ARM;
	}
	if (config & CPUCFG2_MIPSBT) {
		c->options |= LOONGARCH_CPU_LBT_MIPS;
		c->hwcap |= HWCAP_LOONGARCH_LBT_MIPS;
	}
#endif

	config = read_cpucfg(LOONGARCH_CPUCFG6);
	if (config & CPUCFG6_PMP)
		c->options |= LOONGARCH_CPU_PMP;

	config = iocsr_read32(LOONGARCH_IOCSR_FEATURES);
	if (config & IOCSRF_CSRIPI)
		c->options |= LOONGARCH_CPU_CSRIPI;
	if (config & IOCSRF_EXTIOI)
		c->options |= LOONGARCH_CPU_EXTIOI;
	if (config & IOCSRF_FREQSCALE)
		c->options |= LOONGARCH_CPU_SCALEFREQ;
	if (config & IOCSRF_FLATMODE)
		c->options |= LOONGARCH_CPU_FLATMODE;
	if (config & IOCSRF_EIODECODE)
		c->options |= LOONGARCH_CPU_EIODECODE;
	if (config & IOCSRF_VM)
		c->options |= LOONGARCH_CPU_HYPERVISOR;

	config = csr_read32(LOONGARCH_CSR_ASID);
	config = (config & CSR_ASID_BIT) >> CSR_ASID_BIT_SHIFT;
	asid_mask = _GENMASK(config - 1, 0);
	c->asid_mask = asid_mask;

	config = read_csr_prcfg1();
	c->ksave_mask = _GENMASK((config & CSR_CONF1_KSNUM) - 1, 0);
	c->ksave_mask &= ~(EXC_KSAVE_MASK | PERCPU_KSAVE_MASK);

	config = read_csr_prcfg3();
	switch (config & CSR_CONF3_TLBTYPE) {
		case 0:
			c->tlbsizemtlb = 0;
			c->tlbsizestlbsets = 0;
			c->tlbsizestlbways = 0;
			c->tlbsize = 0;
			break;
		case 1:
			c->tlbsizemtlb = ((config & CSR_CONF3_MTLBSIZE) >> CSR_CONF3_MTLBSIZE_SHIFT) + 1;
			c->tlbsizestlbsets = 0;
			c->tlbsizestlbways = 0;
			c->tlbsize = c->tlbsizemtlb + c->tlbsizestlbsets * c->tlbsizestlbways;
			break;
		case 2:
			c->tlbsizemtlb = ((config & CSR_CONF3_MTLBSIZE) >> CSR_CONF3_MTLBSIZE_SHIFT) + 1;
			c->tlbsizestlbsets = 1 << ((config & CSR_CONF3_STLBIDX) >> CSR_CONF3_STLBIDX_SHIFT);
			c->tlbsizestlbways = ((config & CSR_CONF3_STLBWAYS) >> CSR_CONF3_STLBWAYS_SHIFT) + 1;
			c->tlbsize = c->tlbsizemtlb + c->tlbsizestlbsets * c->tlbsizestlbways;
			break;
		default:
			break;
	}
}

static void
identify_cpu_features(u_int cpu, struct cpu_desc *desc)
{
	cpu_probe_common(&desc->cpuinfo);
}

/*
 * Update kernel/user global state based on the feature parsing results, stored
 * in desc.
 *
 * We keep only the subset of values common to all CPUs.
 */
static void
update_global_capabilities(u_int cpu, struct cpu_desc *desc)
{
#define UPDATE_CAP(t, v)				\
	do {						\
		if (cpu == 0) {				\
			(t) = (v);			\
		} else {				\
			(t) &= (v);			\
		}					\
	} while (0)

	/* Update the capabilities exposed to userspace via AT_HWCAP. */
	UPDATE_CAP(elf_hwcap, (u_long)desc->cpuinfo.hwcap);
#undef UPDATE_CAP
}

static void
identify_cpu_ids(struct cpu_desc *desc)
{
	const struct marchid_entry *table = NULL;
	int i;

	desc->cpu_mvendor_name = "Unknown";
	desc->cpu_march_name = "Unknown";

	mvendorid = desc->cpuinfo.processor_id & PRID_COMP_MASK;
	marchid = desc->cpuinfo.arch & LOONGARCH_ARCH_MASK;

	/*
	 * Search for a recognized vendor, and possibly obtain the secondary
	 * table for marchid lookup.
	 */
	for (i = 0; i < nitems(mvendor_ids); i++) {
		if (mvendorid == mvendor_ids[i].mvendor_id) {
			desc->cpu_mvendor_name = mvendor_ids[i].mvendor_name;
			table = mvendor_ids[i].marchid_table;
			break;
		}
	}

	for (i = 0; table[i].march_name != NULL; i++) {
		if (marchid == table[i].march_id) {
			desc->cpu_march_name = table[i].march_name;
			break;
		}
	}
}

void
identify_cpu(u_int cpu)
{
	struct cpu_desc *desc = &cpu_desc[cpu];

	desc->cpuinfo.processor_id = read_cpucfg(LOONGARCH_CPUCFG0);
	desc->cpuinfo.arch = read_cpucfg(LOONGARCH_CPUCFG1);

	identify_cpu_ids(desc);
	identify_cpu_features(cpu, desc);

	update_global_capabilities(cpu, desc);
}

void
printcpuinfo(u_int cpu)
{
	struct cpu_desc *desc;

	desc = &cpu_desc[cpu];

	/* Always print summary line. */
	printf("CPU %-3u: Vendor=%s Core=%s \n", cpu,
	    desc->cpu_mvendor_name, desc->cpu_march_name);

	/* These values are global. */
	if (cpu == 0)
		printf("  marchid=%#lx\n", marchid);
}
