/*-
 * Copyright (c) 2025 Joseph Koshy
 * All rights reserved.
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

divert(-1)
include(TOP`/common/sys/elfconstants.m4')

# The `_' macro defined used to define relocation types has the structure
# `_(SYMBOLIC_NAME, VALUE, ...optional fields...)'.
#
# Expand the `_' macro to a C 'case' statement that returns the symbolic
# name of the R_* constant passed in.
define(`_',`		case $2: return "$1";')

# Generate C code to translate a collection of relocation types.
#
# Usage:
#
#   EXPAND_RELOCATION_TYPES(MACHINE)
#
# where "MACHINE" names a group of relocation types in "elfconstants.m4".
define(`EXPAND_RELOCATION_TYPES',`dnl
switch (r_value) {dnl
DEFINE_$1_RELOCATION_TYPES()dnl
		default: /*FALLTHROUGH*/
			;
		}')
divert(0)dnl

/*-
 * WARNING! WARNING! WARNING!
 *
 * THIS IS A GENERATED FILE.  DO NOT MODIFY.
 *
 * GENERATED FROM: $Id: elftc_get_relocation_type_name.m4 4165 2025-02-05 14:27:19Z jkoshy $
 * GENERATED FROM: VCSID_ELFCONSTANTS_M4
 */

#include <errno.h>
#include <stddef.h>

#include "libelftc.h"

const char *
elftc_get_relocation_type_name(unsigned int e_machine, unsigned int r_value)
{
	switch (e_machine)
	{
	case EM_386:
	case EM_IAMCU:
		EXPAND_RELOCATION_TYPES(386)
		break;

	case EM_AARCH64:
		EXPAND_RELOCATION_TYPES(AARCH64)
		break;

	case EM_ARM:
		EXPAND_RELOCATION_TYPES(ARM)
		break;

	case EM_IA_64:
		EXPAND_RELOCATION_TYPES(IA_64)
		break;

	case EM_LOONGARCH:
		EXPAND_RELOCATION_TYPES(LOONGARCH)
		break;

	case EM_MIPS:
		EXPAND_RELOCATION_TYPES(MIPS)
		break;

	case EM_PPC:
		EXPAND_RELOCATION_TYPES(PPC)
		break;

	case EM_PPC64:
		EXPAND_RELOCATION_TYPES(PPC64)
		break;

	case EM_RISCV:
		EXPAND_RELOCATION_TYPES(RISCV)
		break;

	case EM_S390:
		EXPAND_RELOCATION_TYPES(S390);
		break;

	case EM_SPARC:
	case EM_SPARC32PLUS:
	case EM_SPARCV9:
		EXPAND_RELOCATION_TYPES(SPARC)
		break;

	case EM_VAX:
		EXPAND_RELOCATION_TYPES(VAX)
		break;

	case EM_X86_64:
		EXPAND_RELOCATION_TYPES(X86_64)
		break;

	default:
		break;
	}

	errno = EINVAL;	/* Unknown machine or relocation type. */
	return (NULL);
}
