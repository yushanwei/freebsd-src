/*-
 * Copyright (c) 2015-2017 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 * Copyright (c) 2021 Jessica Clarke <jrtc27@FreeBSD.org>
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

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/proc.h>
#include <sys/cpuset.h>
#include <sys/interrupt.h>
#include <sys/smp.h>
#include <sys/intr.h>

#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/cpufunc.h>
#include <machine/frame.h>

#include "pic_if.h"

#define	INTC_NIRQS	16

struct intc_irqsrc {
	struct intr_irqsrc	isrc;
	u_int			irq;
};

struct intc_softc {
	device_t		dev;
	struct intc_irqsrc	isrcs[INTC_NIRQS];
};

static void
intc_identify(driver_t *driver, device_t parent)
{
}

static int
intc_probe(device_t dev)
{
	return (0);
}

static int
intc_attach(device_t dev)
{
	return (0);
}

static void
intc_disable_intr(device_t dev, struct intr_irqsrc *isrc)
{
}

static void
intc_enable_intr(device_t dev, struct intr_irqsrc *isrc)
{
}

static int
intc_map_intr(device_t dev, struct intr_map_data *data,
    struct intr_irqsrc **isrcp)
{
	return (0);
}

static int
intc_setup_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource *res, struct intr_map_data *data)
{
	return (0);
}

#ifdef SMP
static void
intc_init_secondary(device_t dev, uint32_t rootnum)
{
}
#endif

static device_method_t intc_methods[] = {
	/* Device interface */
	DEVMETHOD(device_identify,	intc_identify),
	DEVMETHOD(device_probe,		intc_probe),
	DEVMETHOD(device_attach,	intc_attach),

	/* Interrupt controller interface */
	DEVMETHOD(pic_disable_intr,	intc_disable_intr),
	DEVMETHOD(pic_enable_intr,	intc_enable_intr),
	DEVMETHOD(pic_map_intr,		intc_map_intr),
	DEVMETHOD(pic_setup_intr,	intc_setup_intr),
#ifdef SMP
	DEVMETHOD(pic_init_secondary,	intc_init_secondary),
#endif

	DEVMETHOD_END
};

DEFINE_CLASS_0(intc, intc_driver, intc_methods, sizeof(struct intc_softc));
EARLY_DRIVER_MODULE(intc, ofwbus, intc_driver, 0, 0,
    BUS_PASS_INTERRUPT + BUS_PASS_ORDER_FIRST);
