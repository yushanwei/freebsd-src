/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 Kyle Evans <kevans@FreeBSD.org>
 * Copyright (c) 2020 Jessica Clarke <jrtc27@FreeBSD.org>
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

/*
 * RISC-V syscon driver. Used as a generic interface by QEMU's virt machine for
 * describing the SiFive test finisher as a power and reset controller.
 */

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/rman.h>
#include <machine/bus.h>

#include <dev/syscon/syscon.h>
#include <dev/syscon/syscon_generic.h>

static int
loongarch_syscon_probe(device_t dev)
{

	if (!ofw_bus_status_okay(dev))
		return (ENXIO);
	if (ofw_bus_search_compatible(dev, compat_data)->ocd_data == 0)
		return (ENXIO);

	device_set_desc(dev, "RISC-V syscon");
	return (BUS_PROBE_DEFAULT);
}

static device_method_t loongarch_syscon_methods[] = {
	DEVMETHOD(device_probe, loongarch_syscon_probe),

	DEVMETHOD_END
};

DEFINE_CLASS_1(loongarch_syscon, loongarch_syscon_driver, loongarch_syscon_methods,
    sizeof(struct syscon_generic_softc), syscon_generic_driver);

/* loongarch_syscon needs to attach prior to syscon_power */
EARLY_DRIVER_MODULE(loongarch_syscon, simplebus, loongarch_syscon_driver, 0, 0,
    BUS_PASS_SCHEDULER + BUS_PASS_ORDER_LAST);
MODULE_VERSION(loongarch_syscon, 1);
