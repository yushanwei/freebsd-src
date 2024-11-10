/*-
 * Copyright (c) 2015-2017 Ruslan Bukin <br@bsdpad.com>
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

#include <sys/param.h>
#include <sys/kdb.h>
#include <sys/kernel.h>
#include <sys/priv.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/conf.h>
#include <sys/cons.h>
#include <sys/consio.h>
#include <sys/tty.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/rman.h>

#include <ddb/ddb.h>

#include <vm/vm.h>
#include <vm/pmap.h>

#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/intr.h>
#include <machine/asm.h>
#include <machine/trap.h>
#include <machine/vmparam.h>

/* bus softc */
struct rcons_softc {
	struct resource		*res[1];
	void			*ihl[1];
	device_t		dev;
};

/* CN Console interface */

static tsw_outwakeup_t loongarchtty_outwakeup;

static struct ttydevsw loongarch_ttydevsw = {
	.tsw_flags	= TF_NOPREFIX,
	.tsw_outwakeup	= loongarchtty_outwakeup,
};

static int			polltime;
static struct callout		loongarch_callout;
static struct tty 		*tp = NULL;

#if defined(KDB)
static int			alt_break_state;
#endif

static void	loongarch_timeout(void *);

static cn_probe_t	loongarch_cnprobe;
static cn_init_t	loongarch_cninit;
static cn_term_t	loongarch_cnterm;
static cn_getc_t	loongarch_cngetc;
static cn_putc_t	loongarch_cnputc;
static cn_grab_t	loongarch_cngrab;
static cn_ungrab_t	loongarch_cnungrab;

CONSOLE_DRIVER(loongarch);

#define	MAX_BURST_LEN		1

static void
loongarch_putc(int c)
{

	sbi_console_putchar(c);
}

#if CHECK_EARLY_PRINTF(sbi)
early_putc_t *early_putc = loongarch_putc;
#endif

static void
cn_drvinit(void *unused)
{

	if (loongarch_consdev.cn_pri != CN_DEAD &&
	    loongarch_consdev.cn_name[0] != '\0') {
		tp = tty_alloc(&loongarch_ttydevsw, NULL);
		tty_init_console(tp, 0);
		tty_makedev(tp, NULL, "%s", "rcons");

		polltime = 1;

		callout_init(&loongarch_callout, 1);
		callout_reset(&loongarch_callout, polltime, loongarch_timeout, NULL);
	}
}

SYSINIT(cndev, SI_SUB_CONFIGURE, SI_ORDER_MIDDLE, cn_drvinit, NULL);

static void
loongarchtty_outwakeup(struct tty *tp)
{
	u_char buf[MAX_BURST_LEN];
	int len;
	int i;

	for (;;) {
		len = ttydisc_getc(tp, buf, sizeof(buf));
		if (len == 0)
			break;

		KASSERT(len == 1, ("tty error"));

		for (i = 0; i < len; i++)
			loongarch_putc(buf[i]);
	}
}

static void
loongarch_timeout(void *v)
{
	int c;

	tty_lock(tp);
	while ((c = loongarch_cngetc(NULL)) != -1)
		ttydisc_rint(tp, c, 0);
	ttydisc_rint_done(tp);
	tty_unlock(tp);

	callout_reset(&loongarch_callout, polltime, loongarch_timeout, NULL);
}

static void
loongarch_cnprobe(struct consdev *cp)
{

	cp->cn_pri = CN_NORMAL;
}

static void
loongarch_cninit(struct consdev *cp)
{

	strcpy(cp->cn_name, "rcons");
}

static void
loongarch_cnterm(struct consdev *cp)
{

}

static void
loongarch_cngrab(struct consdev *cp)
{

}

static void
loongarch_cnungrab(struct consdev *cp)
{

}

static int
loongarch_cngetc(struct consdev *cp)
{
	int ch;

	ch = sbi_console_getchar();
	if (ch > 0 && ch < 0xff) {
#if defined(KDB)
		kdb_alt_break(ch, &alt_break_state);
#endif
		return (ch);
	}

	return (-1);
}

static void
loongarch_cnputc(struct consdev *cp, int c)
{

	loongarch_putc(c);
}

/* Bus interface */

static int
rcons_probe(device_t dev)
{

	device_set_desc(dev, "RISC-V console");

	return (BUS_PROBE_DEFAULT);
}

static int
rcons_attach(device_t dev)
{
	struct rcons_softc *sc;

	if (device_get_unit(dev) != 0)
		return (ENXIO);

	sc = device_get_softc(dev);
	sc->dev = dev;

	bus_attach_children(sc->dev);

	return (0);
}

static device_method_t rcons_methods[] = {
	DEVMETHOD(device_probe,		rcons_probe),
	DEVMETHOD(device_attach,	rcons_attach),

	DEVMETHOD_END
};

static driver_t rcons_driver = {
	"rcons",
	rcons_methods,
	sizeof(struct rcons_softc)
};

DRIVER_MODULE(rcons, nexus, rcons_driver, 0, 0);
