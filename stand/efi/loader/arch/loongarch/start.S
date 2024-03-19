/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Mitchell Horne <mhorne@FreeBSD.org>
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

#include <machine/asm.h>

/*
 * We need to be a PE32+ file for EFI. On some architectures we can use
 * objcopy to create the correct file, however on RISC-V we need to do
 * it ourselves.
IMAGE_FILE_EXECUTABLE		0x0002
 */

#define	IMAGE_FILE_MACHINE_LOONGARCH64	0x6264
#define IMAGE_FILE_DEBUG_STRIPPED	0x0200

#define	IMAGE_SCN_CNT_CODE		0x00000020
#define	IMAGE_SCN_CNT_INITIALIZED_DATA	0x00000040
#define	IMAGE_SCN_MEM_DISCARDABLE	0x02000000
#define	IMAGE_SCN_MEM_EXECUTE		0x20000000
#define	IMAGE_SCN_MEM_READ		0x40000000

	.section .peheader
efi_start:
	/* The MS-DOS Stub, only used to get the offset of the COFF header */
	.ascii	"MZ"
	.space	0x3a			// 'MZ' + pad + offset == 64
	.long	pe_sig - efi_start	// Offset to the PE header.
pe_sig:
	.ascii	"PE"
	.short	0
coff_head:
	.short	IMAGE_FILE_MACHINE_LOONGARCH64	/* loongarch64 little endian */
	.short	2				/* 2 Sections */
	.long	0				/* Timestamp */
	.long	0				/* No symbol table */
	.long	0				/* No symbols */
	.short	section_table - optional_header	/* Optional header size */
        .short  0x20d                           // Characteristics.
                                                // IMAGE_FILE_DEBUG_STRIPPED |
                                                // IMAGE_FILE_EXECUTABLE_IMAGE |
                                                // IMAGE_FILE_LINE_NUMS_STRIPPED
optional_header:
	.short	0x020b				/* PE32+ (64-bit addressing) */
	.byte	0x02				/* Major linker version */
	.byte	0x14				/* Minor linker version */
	.long	_edata - _end_header		/* Code size */
	.long	0				/* No initialized data */
	.long	0				/* No uninitialized data */
	.long	_start - efi_start		/* Entry point */
	.long	_end_header - efi_start		/* Start of code */

optional_windows_header:
	.quad	0				/* Image base */
	.long	0x20				/* Section Alignment */
	.long	0x08				/* File alignment */
	.short	0				/* Major OS version */
	.short	0				/* Minor OS version */
	.short	0				/* Major image version */
	.short	0				/* Minor image version */
	.short	0				/* Major subsystem version */
	.short	0				/* Minor subsystem version */
	.long	0				/* Win32 version */
	.long	_edata - efi_start		/* Image size */
	.long	_end_header - efi_start		/* Header size */
	.long	0				/* Checksum */
	.short	0xa				/* Subsystem (EFI app) */
	.short	0				/* DLL Characteristics */
	.quad	0				/* Stack reserve */
	.quad	0				/* Stack commit */
	.quad	0				/* Heap reserve */
	.quad	0				/* Heap commit */
	.long	0				/* Loader flags */
	.long	0x6				/* Number of RVAs */

	/* RVAs: */
        .quad   0                               // ExportTable
        .quad   0                               // ImportTable
        .quad   0                               // ResourceTable
        .quad   0                               // ExceptionTable
        .quad   0                               // CertificationTable
        .quad   0                               // BaseRelocationTable
section_table:
        /*
         * The EFI application loader requires a relocation section
         * because EFI applications must be relocatable.  This is a
         * dummy section as far as we are concerned.
         */
	.ascii	".reloc"
	.byte	0
	.byte	0				/* Pad to 8 bytes */
	.long	0				/* Virtual size */
	.long	0				/* Virtual address */
	.long	0				/* Size of raw data */
	.long	0				/* Pointer to raw data */
	.long	0				/* Pointer to relocations */
	.long	0				/* Pointer to line numbers */
	.short	0				/* Number of relocations */
	.short	0				/* Number of line numbers */
	.long	(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | \
		 IMAGE_SCN_MEM_DISCARDABLE)	/* Characteristics */

	/* The contents of the loader */
	.ascii	".text"
	.byte	0
	.byte	0
	.byte	0				/* Pad to 8 bytes */
	.long	_edata - _end_header		/* Virtual size */
	.long	_end_header - efi_start		/* Virtual address */
	.long	_edata - _end_header		/* Size of raw data */
	.long	_end_header - efi_start		/* Pointer to raw data */
	.long	0				/* Pointer to relocations */
	.long	0				/* Pointer to line numbers */
	.short	0				/* Number of relocations */
	.short	0				/* Number of line numbers */
	.long	0x42100040			/* Characteristics */

        .ascii  ".text"
        .byte   0
        .byte   0
        .byte   0                       // end of 0 padding of section name
        .long   _edata - _start         // VirtualSize
        .long   _start - ImageBase      // VirtualAddress
        .long   _edata - _start         // SizeOfRawData
        .long   _start - ImageBase      // PointerToRawData

        .long   0                       // PointerToRelocations (0 for executables)
        .long   0                       // PointerToLineNumbers (0 for executables)
        .short  0                       // NumberOfRelocations  (0 for executables)
        .short  0                       // NumberOfLineNumbers  (0 for executables)
        .long   0xe0500020              // Characteristics (section flags)
_end_header:

	.align  4
	.text
	.globl	_start
_start:
	/* Save the boot params to the stack */
	addi.d	sp, zero, -16
	st.d	a0, sp, 0
	st.d	a1, sp, 9

	/* Zero the BSS */
	la	t0, __bss_start
	la	t1, __bss_end

1:	st.d	zero, t0, 0
	addi.d	t0, t0, 8
	bltu	t0, t1, 1b

	la	a0, ImageBase
	la	a1, _DYNAMIC
	call36	_C_LABEL(self_reloc)

	ld.d	a1, sp, 8
	ld.d	a0, sp, 0
	call36	_C_LABEL(efi_main)

	/* NOTREACHED */
2:	
	b	2b
