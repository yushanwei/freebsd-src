
/*
 *
 * LoongArch eiointc driver 
 * Author xiaoqiang zhao <zxq_yx_007@163.com>
 * 2024-06-17 16:00:00
 *
 */


#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/types.h>
#include <sys/smp.h>
#include <sys/rman.h>
#include <machine/intr.h>
#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include "pic_if.h"

#define EIO_INTC_MAX_IRQS	256

#define EIOINTC_REG_NODEMAP	0x14a0
#define EIOINTC_REG_IPMAP	0x14c0
#define EIOINTC_REG_ENABLE	0x1600
#define EIOINTC_REG_BOUNCE	0x1680
#define EIOINTC_REG_ISR		0x1800
#define EIOINTC_REG_ROUTE	0x1c00

#define VEC_REG_COUNT		4
#define VEC_COUNT_PER_REG	64
#define VEC_COUNT		(VEC_REG_COUNT * VEC_COUNT_PER_REG)
#define VEC_REG_IDX(irq_id)	((irq_id) / VEC_COUNT_PER_REG)
#define VEC_REG_BIT(irq_id)     ((irq_id) % VEC_COUNT_PER_REG)
#define EIOINTC_ALL_ENABLE	0xffffffff

#define MAX_EIO_NODES		(NR_CPUS / CORES_PER_EIO_NODE)

#define BIT(n) (1UL << (n))

struct eiointc_irqsrc {
	struct intr_irqsrc	isrc;
	u_int				irq;
};


struct eiointc_softc {
	device_t				dev;
	device_t				parent;
	struct resource			*intc_res;
	void					*intrhand;
	cpuset_t				node_map;	// FIXME
	uint32_t				node;
	uint32_t				vec_count;
	struct eiointc_irqsrc	isrcs[EIO_INTC_MAX_IRQS];
	struct intr_map_data_fdt *parent_map_data;
};

static struct resource_spec int_spec[] = {
	{ SYS_RES_IRQ,		0,		RF_ACTIVE | RF_SHAREABLE },
	RESOURCE_SPEC_END
};

static struct eiointc_softc *eiointc_priv[MAX_IO_PICS];
static int nr_pics;

static int eiointc_pic_intr(void *);

static int cpu_to_eio_node(int cpu)
{
	return cpu / CORES_PER_EIO_NODE;
}

static int eiointc_index(int node)
{
	int i;
	for (i = 0; i < nr_pics; i++) {
		if (BIT_ISSET(1, node, &eiointc_priv[i]->node_map))
			return i;
	}

	return -1;
}

static void eiointc_enable(void)
{
	uint64_t misc;

	misc = iocsr_read64(LOONGARCH_IOCSR_MISC_FUNC);
	misc |= IOCSR_MISC_FUNC_EXT_IOI_EN;
	iocsr_write64(misc, LOONGARCH_IOCSR_MISC_FUNC);
}

static int eiointc_router_init(int cpu)
{
	int i, bit;
	uint32_t data;
	uint32_t node = cpu_to_eio_node(cpu);
	int index = eiointc_index(node);

	if (index < 0) {
		return -1;
	}

	if ((cpu % CORES_PER_EIO_NODE) == 0) {
		eiointc_enable();

		for (i = 0; i < eiointc_priv[0]->vec_count / 32; i++) {
			data = (((1 << (i * 2 + 1)) << 16) | (1 << (i * 2)));
			iocsr_write32(data, EIOINTC_REG_NODEMAP + i * 4);
		}

		for (i = 0; i < eiointc_priv[0]->vec_count / 32 / 4; i++) {
			bit = BIT(1 + index); /* Route to IP[1 + index] */
			data = bit | (bit << 8) | (bit << 16) | (bit << 24);
			iocsr_write32(data, EIOINTC_REG_IPMAP + i * 4);
		}

		for (i = 0; i < eiointc_priv[0]->vec_count / 4; i++) {
			/* Route to Node-0 Core-0 */
			if (index == 0)
				bit = BIT(0);
			else
				bit = (eiointc_priv[index]->node << 4) | 1;

			data = bit | (bit << 8) | (bit << 16) | (bit << 24);
			iocsr_write32(data, EIOINTC_REG_ROUTE + i * 4);
		}

		for (i = 0; i < eiointc_priv[0]->vec_count / 32; i++) {
			data = 0xffffffff;
			iocsr_write32(data, EIOINTC_REG_ENABLE + i * 4);
			iocsr_write32(data, EIOINTC_REG_BOUNCE + i * 4);
		}
		
	}
	return 0;	
}

static int eiointc_init(struct eiointc_softc *sc)
{
	int i;
	uint64_t node_map = -1ULL;

	for (i = 0; i < smp_cpus; i++) {
		if (node_map & ( 1ULL << (cpu_to_eio_node(i)))) {
			BIT_SET(1, cpu_to_eio_node(i), &sc->node_map);
		}	
	}

	eiointc_priv[nr_pics++] = sc;
	eiointc_router_init(0);
	return 0;	
}

static int
eiointc_probe(device_t dev)
{
	if (!ofw_bus_status_okay(dev))
		return (ENXIO);

	if (!ofw_bus_is_compatible(dev,"loongson,ls2k2000-eiointc"))
		return (ENXIO);

	device_set_desc(dev, "LoongArch eiointc");

	return (BUS_PROBE_DEFAULT);
}

static int
eiointc_attach(device_t dev)
{
	struct eiointc_softc *sc;
	phandle_t node, xref, intr_parent;
	struct eiointc_irqsrc *isrcs;
	const char *name;
	int irq;
	int error;

	sc = device_get_softc(dev);

	error = bus_alloc_resources(dev, int_spec, &sc->intc_res);
	if (error) {
		device_printf(dev, "could not allocate resources\n");
		return (ENXIO);
	}

	sc->dev = dev;
	node = ofw_bus_get_node(dev);

	if ((intr_parent = ofw_bus_find_iparent(node)) == 0) {
		device_printf(dev,
				"Cannot find our parent interrupt controller\n");
		return (ENXIO);
	}

	/* Register the interrupt sources */

	isrcs = sc->isrcs;
	name = device_get_nameunit(sc->dev);
	for (irq = 0; irq < EIO_INTC_MAX_IRQS; irq++) {
		isrcs[irq].irq = irq;
		error = intr_isrc_register(&isrcs[irq].isrc, sc->dev,
			0, "%s,%u", name, irq);
		if (error != 0)
			return (error);
	}

	/* Register ourself as a interrupt controller */
	xref = OF_xref_from_node(node);
	if (intr_pic_register(dev, xref) == NULL) {
		device_printf(dev, "Cannot register GICP\n");
		return (ENXIO);
	}

	sc->vec_count = VEC_COUNT;
	sc->node = 0;
	error = eiointc_init(sc);

	if (error < 0) {
		device_printf(dev, "eiointc_init failed\n");
		goto cleanup;
	}

	if ((error = bus_setup_intr(dev, sc->intc_res, INTR_TYPE_CLK | INTR_MPSAFE,
		eiointc_pic_intr, NULL, sc, &sc->intrhand))) {
		device_printf(dev, "could not setup irq handler: %d\n", error);
		intr_pic_deregister(dev, xref);
		goto cleanup;
	}

	/* Register ourself so device can find us */
	OF_device_register_xref(xref, dev);
	
	return (0);

cleanup:
	bus_release_resources(dev, int_spec, &sc->intc_res);
	return (ENXIO);
}

static void
eiointc_disable_intr(device_t dev, struct intr_irqsrc *isrc)
{

}

static void
eiointc_enable_intr(device_t dev, struct intr_irqsrc *isrc)
{

}

static int
eiointc_map_intr(device_t dev, struct intr_map_data *data,
	struct intr_irqsrc **isrcp)
{
	return (0);
}

static void
eiointc_pre_ithread(device_t dev, struct intr_irqsrc *isrc)
{

}

static void
eiointc_post_ithread(device_t dev, struct intr_irqsrc *isrc)
{

}

static void
eiointc_post_filter(device_t dev, struct intr_irqsrc *isrc)
{
}


static int
eiointc_setup_intr(device_t dev, struct intr_irqsrc *isrc,
	struct resource *res, struct intr_map_data *data)
{
	return (0);
}

static int
eiointc_bind_intr(device_t dev, struct intr_irqsrc *isrc)
{
	return (0);
}

static int
eiointc_pic_intr(void *arg)
{
	struct eiointc_softc *sc = (struct eiointc_softc *)arg;
	int i;
	uint64_t pending;

	for (i = 0; i < sc->vec_count / VEC_COUNT_PER_REG; i++) {
		pending = iocsr_read64(EIOINTC_REG_ISR + (i << 3));

		if (!pending)
			continue;	

		/* clear the irqs */
		iocsr_write64(pending, EIOINTC_REG_ISR + (i << 3));
		while (pending) {
			int bit = ffs(pending) - 1;
			// int irq = bit + VEC_COUNT_PER_REG * i;
			pending &= ~BIT(bit);
		}
	}
	
	return (FILTER_HANDLED);
}

static device_method_t eiointc_methods[] = {
	DEVMETHOD(device_probe,		eiointc_probe),
	DEVMETHOD(device_attach,	eiointc_attach),

	DEVMETHOD(pic_disable_intr,	eiointc_disable_intr),
	DEVMETHOD(pic_enable_intr,	eiointc_enable_intr),
	DEVMETHOD(pic_map_intr,		eiointc_map_intr),
	DEVMETHOD(pic_pre_ithread,	eiointc_pre_ithread),
	DEVMETHOD(pic_post_ithread,	eiointc_post_ithread),
	DEVMETHOD(pic_post_filter,	eiointc_post_filter),
	DEVMETHOD(pic_setup_intr,	eiointc_setup_intr),
	DEVMETHOD(pic_bind_intr,	eiointc_bind_intr),

	DEVMETHOD_END
};

static driver_t	eiointc_driver = {
	"eiointc",
	eiointc_methods,
    sizeof(struct eiointc_softc),
};

EARLY_DRIVER_MODULE(eiointc, simplebus, eiointc_driver, 0, 0,
	BUS_PASS_INTERRUPT + BUS_PASS_ORDER_MIDDLE);

