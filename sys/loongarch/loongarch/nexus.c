/*-
 * Copyright 1998 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * This code implements a `root nexus' for RISC-V Architecture
 * machines.  The function of the root nexus is to serve as an
 * attachment point for both processors and buses, and to manage
 * resources which are common to all of them.  In particular,
 * this code implements the core resource managers for interrupt
 * requests, DMA requests (which rightfully should be a part of the
 * ISA code but it's easier to do it here for now), I/O port addresses,
 * and I/O memory address space.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <machine/bus.h>
#include <sys/rman.h>
#include <sys/interrupt.h>

#include <machine/vmparam.h>
#include <machine/pcb.h>
#include <vm/vm.h>
#include <vm/pmap.h>

#include <machine/resource.h>
#include <machine/intr.h>

#include "opt_platform.h"

#include "ofw_bus_if.h"

extern struct bus_space memmap_bus;

static MALLOC_DEFINE(M_NEXUSDEV, "nexusdev", "Nexus device");

struct nexus_device {
	struct resource_list	nx_resources;
};

#define DEVTONX(dev)	((struct nexus_device *)device_get_ivars(dev))

static struct rman mem_rman;
static struct rman irq_rman;

static int nexus_attach(device_t);

static	int nexus_print_child(device_t, device_t);
static	device_t nexus_add_child(device_t, u_int, const char *, int);
static	struct resource *nexus_alloc_resource(device_t, device_t, int, int *,
    u_long, u_long, u_long, u_int);
static	int nexus_activate_resource(device_t, device_t, int, int,
    struct resource *);
static int nexus_config_intr(device_t dev, int irq, enum intr_trigger trig,
    enum intr_polarity pol);
static struct resource_list *nexus_get_reslist(device_t, device_t);
static	int nexus_set_resource(device_t, device_t, int, int, u_long, u_long);
static	int nexus_deactivate_resource(device_t, device_t, int, int,
    struct resource *);

static int nexus_setup_intr(device_t dev, device_t child, struct resource *res,
    int flags, driver_filter_t *filt, driver_intr_t *intr, void *arg, void **cookiep);
static int nexus_teardown_intr(device_t, device_t, struct resource *, void *);

static int nexus_ofw_map_intr(device_t dev, device_t child, phandle_t iparent,
    int icells, pcell_t *intr);

static device_method_t nexus_methods[] = {
	/* Device interface */
	DEVMETHOD(device_attach,	nexus_attach),

	/* OFW interface */
	DEVMETHOD(ofw_bus_map_intr,	nexus_ofw_map_intr),

	/* Bus interface */
	DEVMETHOD(bus_print_child,	nexus_print_child),
	DEVMETHOD(bus_add_child,	nexus_add_child),
	DEVMETHOD(bus_alloc_resource,	nexus_alloc_resource),
	DEVMETHOD(bus_activate_resource,	nexus_activate_resource),
	DEVMETHOD(bus_config_intr,	nexus_config_intr),
	DEVMETHOD(bus_get_resource_list, nexus_get_reslist),
	DEVMETHOD(bus_set_resource,	nexus_set_resource),
	DEVMETHOD(bus_deactivate_resource,	nexus_deactivate_resource),
	DEVMETHOD(bus_setup_intr,	nexus_setup_intr),
	DEVMETHOD(bus_teardown_intr,	nexus_teardown_intr),

	{ 0, 0 }
};

static int
nexus_attach(device_t dev)
{

	mem_rman.rm_start = 0;
	mem_rman.rm_end = ~0ul;
	mem_rman.rm_type = RMAN_ARRAY;
	mem_rman.rm_descr = "I/O memory addresses";
	if (rman_init(&mem_rman) || rman_manage_region(&mem_rman, 0, ~0))
		panic("nexus_attach mem_rman");
	irq_rman.rm_start = 0;
	irq_rman.rm_end = ~0ul;
	irq_rman.rm_type = RMAN_ARRAY;
	irq_rman.rm_descr = "Interrupts";
	if (rman_init(&irq_rman) || rman_manage_region(&irq_rman, 0, ~0))
		panic("nexus_attach irq_rman");

	nexus_add_child(dev, 10, "ofwbus", 0);

	bus_generic_probe(dev);
	bus_generic_attach(dev);

	return (0);
}

static int
nexus_print_child(device_t bus, device_t child)
{
	int retval = 0;

	retval += bus_print_child_header(bus, child);
	retval += printf("\n");

	return (retval);
}

static device_t
nexus_add_child(device_t bus, u_int order, const char *name, int unit)
{
	device_t child;
	struct nexus_device *ndev;

	ndev = malloc(sizeof(struct nexus_device), M_NEXUSDEV, M_NOWAIT|M_ZERO);
	if (!ndev)
		return (0);
	resource_list_init(&ndev->nx_resources);

	child = device_add_child_ordered(bus, order, name, unit);

	/* should we free this in nexus_child_detached? */
	device_set_ivars(child, ndev);

	return (child);
}


/*
 * Allocate a resource on behalf of child.  NB: child is usually going to be a
 * child of one of our descendants, not a direct child of nexus0.
 * (Exceptions include footbridge.)
 */
static struct resource *
nexus_alloc_resource(device_t bus, device_t child, int type, int *rid,
    u_long start, u_long end, u_long count, u_int flags)
{
	struct nexus_device *ndev = DEVTONX(child);
	struct resource *rv;
	struct resource_list_entry *rle;
	struct rman *rm;
	int needactivate = flags & RF_ACTIVE;

	/*
	 * If this is an allocation of the "default" range for a given
	 * RID, and we know what the resources for this device are
	 * (ie. they aren't maintained by a child bus), then work out
	 * the start/end values.
	 */
	if ((start == 0UL) && (end == ~0UL) && (count == 1)) {
		if (device_get_parent(child) != bus || ndev == NULL)
			return(NULL);
		rle = resource_list_find(&ndev->nx_resources, type, *rid);
		if (rle == NULL)
			return(NULL);
		start = rle->start;
		end = rle->end;
		count = rle->count;
	}

	switch (type) {
	case SYS_RES_IRQ:
		rm = &irq_rman;
		break;

	case SYS_RES_MEMORY:
	case SYS_RES_IOPORT:
		rm = &mem_rman;
		break;

	default:
		return (NULL);
	}

	rv = rman_reserve_resource(rm, start, end, count, flags, child);
	if (rv == 0)
		return (NULL);

	rman_set_rid(rv, *rid);
	rman_set_bushandle(rv, rman_get_start(rv));

	if (needactivate) {
		if (bus_activate_resource(child, type, *rid, rv)) {
			rman_release_resource(rv);
			return (NULL);
		}
	}

	return (rv);
}

static int
nexus_config_intr(device_t dev, int irq, enum intr_trigger trig,
    enum intr_polarity pol)
{

	return (riscv_config_intr(irq, trig, pol));
}

static int
nexus_setup_intr(device_t dev, device_t child, struct resource *res, int flags,
    driver_filter_t *filt, driver_intr_t *intr, void *arg, void **cookiep)
{
	int error;

	if ((rman_get_flags(res) & RF_SHAREABLE) == 0)
		flags |= INTR_EXCL;

	/* We depend here on rman_activate_resource() being idempotent. */
	error = rman_activate_resource(res);
	if (error)
		return (error);

	error = riscv_setup_intr(device_get_nameunit(child), filt, intr,
	    arg, rman_get_start(res), flags, cookiep);

	return (error);
}

static int
nexus_teardown_intr(device_t dev, device_t child, struct resource *r, void *ih)
{

	return (riscv_teardown_intr(ih));
}

static int
nexus_activate_resource(device_t bus, device_t child, int type, int rid,
    struct resource *r)
{
	int err;
	bus_addr_t paddr;
	bus_size_t psize;
	bus_space_handle_t vaddr;

	if ((err = rman_activate_resource(r)) != 0)
		return (err);

	/*
	 * If this is a memory resource, map it into the kernel.
	 */
	if (type == SYS_RES_MEMORY || type == SYS_RES_IOPORT) {
		paddr = (bus_addr_t)rman_get_start(r);
		psize = (bus_size_t)rman_get_size(r);
		err = bus_space_map(&memmap_bus, paddr, psize, 0, &vaddr);
		if (err != 0) {
			rman_deactivate_resource(r);
			return (err);
		}
		rman_set_bustag(r, &memmap_bus);
		rman_set_virtual(r, (void *)vaddr);
		rman_set_bushandle(r, vaddr);
	}
	return (0);
}

static struct resource_list *
nexus_get_reslist(device_t dev, device_t child)
{
	struct nexus_device *ndev = DEVTONX(child);

	return (&ndev->nx_resources);
}

static int
nexus_set_resource(device_t dev, device_t child, int type, int rid,
    u_long start, u_long count)
{
	struct nexus_device	*ndev = DEVTONX(child);
	struct resource_list	*rl = &ndev->nx_resources;

	/* XXX this should return a success/failure indicator */
	resource_list_add(rl, type, rid, start, start + count - 1, count);

	return(0);
}


static int
nexus_deactivate_resource(device_t bus, device_t child, int type, int rid,
    struct resource *r)
{
	bus_size_t psize;
	bus_space_handle_t vaddr;

	psize = (bus_size_t)rman_get_size(r);
	vaddr = rman_get_bushandle(r);

	if (vaddr != 0) {
		bus_space_unmap(&memmap_bus, vaddr, psize);
		rman_set_virtual(r, NULL);
		rman_set_bushandle(r, 0);
	}

	return (rman_deactivate_resource(r));
}


static int
nexus_ofw_map_intr(device_t dev, device_t child, phandle_t iparent, int icells,
    pcell_t *intr)
{
	int irq;

	if (icells == 3) {
		irq = intr[1];
		if (intr[0] == 0)
			irq += 32; /* SPI */
		else
			irq += 16; /* PPI */
	} else
		irq = intr[0];

	return (irq);
}
