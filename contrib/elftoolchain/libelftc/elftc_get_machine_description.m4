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

# The `_' macro defined by DEFINE_ELF_MACHINE_TYPES() has structure
# `_(SYMBOLIC_NAME, VALUE, DESCRIPTION)'.
#
# Have this macro expand to a 'case' statement that returns the description
# for the EM_* value passed in.	
define(`_',`	case $2: /*$1*/ return "$3";')
divert(0)dnl

/*-
 * WARNING! WARNING! WARNING!
 *
 * THIS IS A GENERATED FILE.  DO NOT MODIFY.
 *
 * GENERATED FROM: $Id: elftc_get_machine_description.m4 4110 2025-01-27 09:10:02Z jkoshy $
 * GENERATED FROM: VCSID_ELFCONSTANTS_M4
 */

#include <errno.h>
#include <stddef.h>

#include "libelftc.h"

const char *
elftc_get_machine_description(unsigned int e_machine)
{
	switch (e_machine)
	{
	DEFINE_ELF_MACHINE_TYPES()

	default:
		errno = EINVAL;
		return (NULL);
	}
}
