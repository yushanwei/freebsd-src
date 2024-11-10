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
#include <machine/cbo.h>

const char machine[] = "loongarch";

SYSCTL_CONST_STRING(_hw, HW_MACHINE, machine, CTLFLAG_RD | CTLFLAG_CAPRD,
    machine, "Machine class");

/* Hardware implementation info. These values may be empty. */
register_t mvendorid;	/* The CPU's JEDEC vendor ID */
register_t marchid;	/* The architecture ID */
register_t mimpid;	/* The implementation ID */

u_int mmu_caps;

/* Supervisor-mode extension support. */
bool has_hyp;
bool __read_frequently has_sstc;
bool __read_frequently has_sscofpmf;
bool has_svpbmt;

/* Z-extensions support. */
bool has_zicbom;
bool has_zicboz;
bool has_zicbop;

struct cpu_desc {
	const char	*cpu_mvendor_name;
	const char	*cpu_march_name;
	u_int		isa_extensions;		/* Single-letter extensions. */
	u_int		mmu_caps;
	u_int		smode_extensions;
#define	 SV_SSTC	(1 << 0)
#define	 SV_SVNAPOT	(1 << 1)
#define	 SV_SVPBMT	(1 << 2)
#define	 SV_SVINVAL	(1 << 3)
#define	 SV_SSCOFPMF	(1 << 4)
	u_int		z_extensions;		/* Multi-letter extensions. */
#define	 Z_ZICBOM	(1 << 0)
#define	 Z_ZICBOZ	(1 << 1)
#define	 Z_ZICBOP	(1 << 2)
	int		cbom_block_size;
	int		cboz_block_size;
};

struct cpu_desc cpu_desc[MAXCPU];

/*
 * Micro-architecture tables.
 */
struct marchid_entry {
	register_t	march_id;
	const char	*march_name;
};

/*
 * Known CPU vendor/manufacturer table.
 */
static const struct {
	register_t			mvendor_id;
	const char			*mvendor_name;
	const struct marchid_entry	*marchid_table;
} mvendor_ids[] = {
};

/*
 * The ISA string describes the complete set of instructions supported by a
 * RISC-V CPU. The string begins with a small prefix (e.g. rv64) indicating the
 * base ISA. It is followed first by single-letter ISA extensions, and then
 * multi-letter ISA extensions.
 *
 * Underscores are used mainly to separate consecutive multi-letter extensions,
 * but may optionally appear between any two extensions. An extension may be
 * followed by a version number, in the form of 'Mpm', where M is the
 * extension's major version number, and 'm' is the minor version number.
 *
 * The format is described in detail by the "ISA Extension Naming Conventions"
 * chapter of the unprivileged spec.
 */
#define	ISA_PREFIX		("rv" __XSTRING(__loongarch_xlen))
#define	ISA_PREFIX_LEN		(sizeof(ISA_PREFIX) - 1)

static __inline int
parse_ext_s(struct cpu_desc *desc, char *isa, int idx, int len)
{
#define	CHECK_S_EXT(str, flag)						\
	do {								\
		if (strncmp(&isa[idx], (str),				\
		    MIN(strlen(str), len - idx)) == 0) {		\
			desc->smode_extensions |= flag;			\
			return (idx + strlen(str));			\
		}							\
	} while (0)

	/* Check for known/supported extensions. */
	CHECK_S_EXT("sstc",	SV_SSTC);
	CHECK_S_EXT("svnapot",	SV_SVNAPOT);
	CHECK_S_EXT("svpbmt",	SV_SVPBMT);
	CHECK_S_EXT("svinval",	SV_SVINVAL);
	CHECK_S_EXT("sscofpmf",	SV_SSCOFPMF);

#undef CHECK_S_EXT

	/*
	 * Proceed to the next multi-letter extension or the end of the
	 * string.
	 */
	while (isa[idx] != '_' && idx < len) {
		idx++;
	}

	return (idx);
}

static __inline int
parse_ext_x(struct cpu_desc *desc __unused, char *isa, int idx, int len)
{
	/*
	 * Proceed to the next multi-letter extension or the end of the
	 * string.
	 */
	while (isa[idx] != '_' && idx < len) {
		idx++;
	}

	return (idx);
}

static __inline int
parse_ext_z(struct cpu_desc *desc __unused, char *isa, int idx, int len)
{
#define	CHECK_Z_EXT(str, flag)						\
	do {								\
		if (strncmp(&isa[idx], (str),				\
		    MIN(strlen(str), len - idx)) == 0) {		\
			desc->z_extensions |= flag;			\
			return (idx + strlen(str));			\
		}							\
	} while (0)

	/* Check for known/supported extensions. */
	CHECK_Z_EXT("zicbom",	Z_ZICBOM);
	CHECK_Z_EXT("zicboz",	Z_ZICBOZ);
	CHECK_Z_EXT("zicbop",	Z_ZICBOP);

#undef CHECK_Z_EXT
	/*
	 * Proceed to the next multi-letter extension or the end of the
	 * string.
	 */
	while (isa[idx] != '_' && idx < len) {
		idx++;
	}

	return (idx);
}

static __inline int
parse_ext_version(char *isa, int idx, u_int *majorp __unused,
    u_int *minorp __unused)
{
	/* Major version. */
	while (isdigit(isa[idx]))
		idx++;

	if (isa[idx] != 'p')
		return (idx);
	else
		idx++;

	/* Minor version. */
	while (isdigit(isa[idx]))
		idx++;

	return (idx);
}

/*
 * Parse the ISA string, building up the set of HWCAP bits as they are found.
 */
static int
parse_loongarch_isa(struct cpu_desc *desc, char *isa, int len)
{
	return (0);
}

static void
identify_cpu_features(u_int cpu, struct cpu_desc *desc)
{
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
	UPDATE_CAP(elf_hwcap, (u_long)desc->isa_extensions);

	/*
	 * MMU capabilities, e.g. Sv48.
	 */
	UPDATE_CAP(mmu_caps, desc->mmu_caps);


#undef UPDATE_CAP
}

static void
identify_cpu_ids(struct cpu_desc *desc)
{
	const struct marchid_entry *table = NULL;
	int i;

	desc->cpu_mvendor_name = "Unknown";
	desc->cpu_march_name = "Unknown";

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

static void
handle_cpu_quirks(u_int cpu, struct cpu_desc *desc)
{
}

void
identify_cpu(u_int cpu)
{
	struct cpu_desc *desc = &cpu_desc[cpu];

	identify_cpu_ids(desc);
	identify_cpu_features(cpu, desc);

	update_global_capabilities(cpu, desc);
	handle_cpu_quirks(cpu, desc);

	if (has_zicbom && cpu == 0)
		cbo_zicbom_setup_cache(desc->cbom_block_size);
}

void
printcpuinfo(u_int cpu)
{
	struct cpu_desc *desc;
	u_int hart;

	desc = &cpu_desc[cpu];
	hart = pcpu_find(cpu)->pc_hart;

	/* XXX: check this here so we are guaranteed to have console output. */
	KASSERT(desc->isa_extensions != 0,
	    ("Empty extension set for CPU %u, did parsing fail?", cpu));

	/*
	 * Suppress the output of some fields in the common case of identical
	 * CPU features.
	 */
#define	SHOULD_PRINT(_field)	\
    (cpu == 0 || desc[0]._field != desc[-1]._field)

	/* Always print summary line. */
	printf("CPU %-3u: Vendor=%s Core=%s (Hart %u)\n", cpu,
	    desc->cpu_mvendor_name, desc->cpu_march_name, hart);

	/* These values are global. */
	if (cpu == 0)
		printf("  marchid=%#lx, mimpid=%#lx\n", marchid, mimpid);

	if (SHOULD_PRINT(mmu_caps)) {
		printf("  MMU: %#b\n", desc->mmu_caps,
		    "\020"
		    "\01Sv39"
		    "\02Sv48"
		    "\03Sv57");
	}

	if (SHOULD_PRINT(isa_extensions)) {
		printf("  ISA: %#b\n", desc->isa_extensions,
		    "\020"
		    "\01Atomic"
		    "\03Compressed"
		    "\04Double"
		    "\06Float"
		    "\10Hypervisor"
		    "\15Mult/Div");
	}

	if (SHOULD_PRINT(smode_extensions)) {
		printf("  S-mode Extensions: %#b\n", desc->smode_extensions,
		    "\020"
		    "\01Sstc"
		    "\02Svnapot"
		    "\03Svpbmt"
		    "\04Svinval"
		    "\05Sscofpmf");
	}

#undef SHOULD_PRINT
}
