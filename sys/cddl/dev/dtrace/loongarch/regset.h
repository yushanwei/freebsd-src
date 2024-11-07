/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 *
 */
/*
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1990, 1991 UNIX System Laboratories, Inc. */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T		*/
/*	All Rights Reserved	*/

#ifndef	_REGSET_H
#define	_REGSET_H

/*
 */

#ifdef __cplusplus
extern "C" {
#endif

/* LoongArch Registers */
#define REG_ZERO        0x0
#define REG_RA          0x1
#define REG_TP          0x2
#define REG_SP          0x3
#define REG_A0          0x4 /* Reused as V0 for return value */
#define REG_A1          0x5 /* Reused as V1 for return value */
#define REG_A2          0x6
#define REG_A3          0x7
#define REG_A4          0x8
#define REG_A5          0x9
#define REG_A6          0xa
#define REG_A7          0xb
#define REG_T0          0xc
#define REG_T1          0xd
#define REG_T2          0xe
#define REG_T3          0xf
#define REG_T4          0x10
#define REG_T5          0x11
#define REG_T6          0x12
#define REG_T7          0x13
#define REG_T8          0x14
#define REG_U0          0x15 /* Kernel uses it as percpu base */
#define REG_FP          0x16
#define REG_S0          0x17
#define REG_S1          0x18
#define REG_S2          0x19
#define REG_S3          0x1a
#define REG_S4          0x1b
#define REG_S5          0x1c
#define REG_S6          0x1d
#define REG_S7          0x1e
#define REG_S8          0x1f

#ifdef	__cplusplus
}
#endif

#endif	/* _REGSET_H */
