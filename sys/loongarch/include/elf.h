/*-
 * Copyright (c) 1996-1997 John D. Polstra.
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

#ifndef	_MACHINE_ELF_H_
#define	_MACHINE_ELF_H_

/*
 * ELF definitions for the RISC-V architecture.
 */

#include <sys/elf32.h>	/* Definitions common to all 32 bit architectures. */
#include <sys/elf64.h>	/* Definitions common to all 64 bit architectures. */

/* Used by <sys/elf_generic.h> */
#ifndef __loongarch_lp64
#define	__ELF_WORD_SIZE	32
#else
#define	__ELF_WORD_SIZE	64
#endif
#include <sys/elf_generic.h>

/*
 * Auxiliary vector entries for passing information to the interpreter.
 */

typedef struct {	/* Auxiliary vector entry on initial stack */
	int	a_type;			/* Entry type. */
	union {
		int	a_val;		/* Integer value. */
	} a_un;
} Elf32_Auxinfo;

typedef struct {	/* Auxiliary vector entry on initial stack */
	long	a_type;			/* Entry type. */
	union {
		long	a_val;		/* Integer value. */
		void	*a_ptr;		/* Address. */
		void	(*a_fcn)(void);	/* Function pointer (not used). */
	} a_un;
} Elf64_Auxinfo;

__ElfType(Auxinfo);

#define ELF_ARCH        EM_LOONGARCH
#define	ELF_ARCH32	EM_LOONGARCH

#define	ELF_MACHINE_OK(x) ((x) == (ELF_ARCH))

/* Define "machine" characteristics */
#define	ELF_TARG_CLASS	ELFCLASS64
#define	ELF_TARG_DATA	ELFDATA2LSB
#define	ELF_TARG_MACH	EM_LOONGARCH
#define	ELF_TARG_VER	1

/* TODO: set correct value */
#define	ET_DYN_LOAD_ADDR 0x100000

/* Flags passed in AT_HWCAP */
#define HWCAP_LOONGARCH_CPUCFG          (1 << 0)
#define HWCAP_LOONGARCH_LAM             (1 << 1)
#define HWCAP_LOONGARCH_UAL             (1 << 2)
#define HWCAP_LOONGARCH_FPU             (1 << 3)
#define HWCAP_LOONGARCH_LSX             (1 << 4)
#define HWCAP_LOONGARCH_LASX            (1 << 5)
#define HWCAP_LOONGARCH_CRC32           (1 << 6)
#define HWCAP_LOONGARCH_COMPLEX         (1 << 7)
#define HWCAP_LOONGARCH_CRYPTO          (1 << 8)
#define HWCAP_LOONGARCH_LVZ             (1 << 9)
#define HWCAP_LOONGARCH_LBT_X86         (1 << 10)
#define HWCAP_LOONGARCH_LBT_ARM         (1 << 11)
#define HWCAP_LOONGARCH_LBT_MIPS        (1 << 12)
#define HWCAP_LOONGARCH_PTW             (1 << 13)

#endif /* !_MACHINE_ELF_H_ */
