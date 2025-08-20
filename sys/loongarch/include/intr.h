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

#ifndef	_MACHINE_INTR_MACHDEP_H_
#define	_MACHINE_INTR_MACHDEP_H_

#ifndef	NIRQ
#define	NIRQ			576
#endif

#define CORES_PER_EIO_NODE  4

#define LOONGSON_CPU_UART0_VEC    10 /* CPU UART0 */
#define LOONGSON_CPU_THSENS_VEC   14 /* CPU Thsens */
#define LOONGSON_CPU_HT0_VEC    16 /* CPU HT0 irq vector base number */
#define LOONGSON_CPU_HT1_VEC    24 /* CPU HT1 irq vector base number */

/* IRQ number definitions */
#define LOONGSON_LPC_IRQ_BASE   0
#define LOONGSON_LPC_LAST_IRQ   (LOONGSON_LPC_IRQ_BASE + 15)

#define LOONGSON_CPU_IRQ_BASE   16
#define LOONGSON_CPU_LAST_IRQ   (LOONGSON_CPU_IRQ_BASE + 14)

#define LOONGSON_PCH_IRQ_BASE   64
#define LOONGSON_PCH_ACPI_IRQ   (LOONGSON_PCH_IRQ_BASE + 47)
#define LOONGSON_PCH_LAST_IRQ   (LOONGSON_PCH_IRQ_BASE + 64 - 1)

#define LOONGSON_MSI_IRQ_BASE   (LOONGSON_PCH_IRQ_BASE + 64)
#define LOONGSON_MSI_LAST_IRQ   (LOONGSON_PCH_IRQ_BASE + 256 - 1)

#define GSI_MIN_LPC_IRQ   LOONGSON_LPC_IRQ_BASE
#define GSI_MAX_LPC_IRQ   (LOONGSON_LPC_IRQ_BASE + 16 - 1)
#define GSI_MIN_CPU_IRQ   LOONGSON_CPU_IRQ_BASE
#define GSI_MAX_CPU_IRQ   (LOONGSON_CPU_IRQ_BASE + 48 - 1)
#define GSI_MIN_PCH_IRQ   LOONGSON_PCH_IRQ_BASE
#define GSI_MAX_PCH_IRQ   (LOONGSON_PCH_IRQ_BASE + 256 - 1)

#ifndef LOCORE
enum {
        IRQ_SWI0,
        IRQ_SWI1,
        IRQ_HWI0,
        IRQ_HWI1,
        IRQ_HWI2,
        IRQ_HWI3,
        IRQ_HWI4,
        IRQ_HWI5,
        IRQ_HWI6,
        IRQ_HWI7,
        IRQ_PCOV, /* Performance counter overflow */
        IRQ_TI,   /* Timer */
        IRQ_IPI,
        IRQ_NMI,
        INTC_NIRQS,
};
#endif /* !LOCORE */

#define	INTR_ROOT_IRQ	0
#define	INTR_ROOT_COUNT	1

#endif /* !_MACHINE_INTR_MACHDEP_H_ */
