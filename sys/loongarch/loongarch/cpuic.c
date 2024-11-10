
/*
 *
 * LoongArch cpuintc driver 
 * Author xiaoqiang zhao <zxq_yx_007@163.com>
 * 2024-06-17 16:00:00
 *
 */


#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/types.h>
#include <sys/cpu.h>
#include <sys/proc.h>
#include <machine/intr.h>

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

struct cpuintc_irqsrc {
	struct intr_irqsrc	isrc;
	u_int				irq;
	uint32_t			node;
	uint32_t			vec_count;
};

struct cpuintc_softc {
	device_t				dev;
	device_t				parent;
	struct resource			*intc_res;
	struct cpuintc_irqsrc	isrcs[EIO_INTC_MAX_IRQS];
};

static int
cpuintc_probe(device_t dev)
{
	if (!ofw_bus_status_okay(dev))
		return (ENXIO);

	if (!ofw_bus_is_compatible(dev,"loongson,cpu-interrupt-controller"))
		return (ENXIO);

	device_set_desc(dev, "LoongArch intc");

	return (BUS_PROBE_DEFAULT);
}

int loongarch_pic_intr(void *arg)
{
	//struct cpuintc_softc *sc = arg;
	struct trapframe *tf;

	tf = curthread->td_intr_frame;

	loongarch_cpu_intr(tf);
	return (FILTER_HANDLED);
}

static int
cpuintc_attach(device_t dev)
{
	struct cpuintc_softc *sc;
	phandle_t node, xref;

	sc = device_get_softc(dev);
	sc->dev = dev;
	node = ofw_bus_get_node(dev);

	/* Register ourself as a interrupt controller */
	xref = OF_xref_from_node(node);
	if (intr_pic_register(dev, xref) == NULL) {
		device_printf(dev, "Cannot register PIC\n");
		return (ENXIO);
	}

	OF_device_register_xref(xref, dev);

	/* Claim our root controller role */
	if (intr_pic_claim_root(dev, xref, loongarch_pic_intr, sc, 0) != 0) {
		device_printf(dev, "count not set PIC as root\n");
		intr_pic_deregister(dev, xref);
		return (ENXIO);
	}
	
	return (0);
}

static void
cpuintc_disable_intr(device_t dev, struct intr_irqsrc *isrc)
{

}

static void
cpuintc_enable_intr(device_t dev, struct intr_irqsrc *isrc)
{

}

static int
cpuintc_map_intr(device_t dev, struct intr_map_data *data,
	struct intr_irqsrc **isrcp)
{
	return (0);
}

static void
cpuintc_pre_ithread(device_t dev, struct intr_irqsrc *isrc)
{

}

static void
cpuintc_post_ithread(device_t dev, struct intr_irqsrc *isrc)
{

}

static void
cpuintc_post_filter(device_t dev, struct intr_irqsrc *isrc)
{
}


static int
cpuintc_setup_intr(device_t dev, struct intr_irqsrc *isrc,
	struct resource *res, struct intr_map_data *data)
{
	return (0);
}

static int
cpuintc_bind_intr(device_t dev, struct intr_irqsrc *isrc)
{
	return (0);
}

static device_method_t cpuintc_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		cpuintc_probe),
	DEVMETHOD(device_attach,	cpuintc_attach),

	/* Interrupt controller interface */
	DEVMETHOD(pic_disable_intr,	cpuintc_disable_intr),
	DEVMETHOD(pic_enable_intr,	cpuintc_enable_intr),
	DEVMETHOD(pic_map_intr,		cpuintc_map_intr),
	DEVMETHOD(pic_pre_ithread,	cpuintc_pre_ithread),
	DEVMETHOD(pic_post_ithread,	cpuintc_post_ithread),
	DEVMETHOD(pic_post_filter,	cpuintc_post_filter),
	DEVMETHOD(pic_setup_intr,	cpuintc_setup_intr),
	DEVMETHOD(pic_bind_intr,	cpuintc_bind_intr),

	DEVMETHOD_END
};

static driver_t	cpuintc_driver = {
	"cpuintc",
	cpuintc_methods,
    sizeof(struct cpuintc_softc),
};

EARLY_DRIVER_MODULE(cpuintc, ofwbus, cpuintc_driver, 0, 0,
	BUS_PASS_INTERRUPT);

