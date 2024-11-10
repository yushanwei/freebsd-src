/*-
 * Copyright (c) 2014 Andrew Turner
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

#include "opt_ddb.h"
#include "opt_kstack_pages.h"
#include "opt_platform.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/boot.h>
#include <sys/buf.h>
#include <sys/bus.h>
#include <sys/cons.h>
#include <sys/cpu.h>
#include <sys/efi_map.h>
#include <sys/exec.h>
#include <sys/imgact.h>
#include <sys/kdb.h>
#include <sys/kernel.h>
#include <sys/ktr.h>
#include <sys/limits.h>
#include <sys/linker.h>
#include <sys/msgbuf.h>
#include <sys/pcpu.h>
#include <sys/physmem.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/reboot.h>
#include <sys/reg.h>
#include <sys/rwlock.h>
#include <sys/sched.h>
#include <sys/signalvar.h>
#include <sys/syscallsubr.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <sys/tslog.h>
#include <sys/ucontext.h>
#include <sys/vmmeter.h>

#include <vm/vm.h>
#include <vm/vm_param.h>
#include <vm/vm_kern.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_phys.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_pager.h>

#include <machine/cpu.h>
#include <machine/fpe.h>
#include <machine/intr.h>
#include <machine/kdb.h>
#include <machine/machdep.h>
#include <machine/metadata.h>
#include <machine/pcb.h>
#include <machine/pte.h>
#include <machine/loongarchreg.h>
#include <machine/trap.h>
#include <machine/vmparam.h>

#ifdef DDB
#include <ddb/ddb.h>
#endif

struct pcpu __pcpu[MAXCPU];

static struct trapframe proc0_tf;

int early_boot = 1;
int cold = 1;

#define	DTB_SIZE_MAX	(1024 * 1024)

struct kva_md_info kmi;

#define BOOT_HART_INVALID	0xffffffff
uint32_t boot_hart = BOOT_HART_INVALID;	/* The hart we booted on. */

cpuset_t all_harts;

extern int *end;

static char static_kenv[PAGE_SIZE];

static void
cpu_startup(void *dummy)
{

	printcpuinfo(0);

	printf("real memory  = %ju (%ju MB)\n", ptoa((uintmax_t)realmem),
	    ptoa((uintmax_t)realmem) / (1024 * 1024));

	/*
	 * Display any holes after the first chunk of extended memory.
	 */
	if (bootverbose) {
		int indx;

		printf("Physical memory chunk(s):\n");
		for (indx = 0; phys_avail[indx + 1] != 0; indx += 2) {
			vm_paddr_t size;

			size = phys_avail[indx + 1] - phys_avail[indx];
			printf(
			    "0x%016jx - 0x%016jx, %ju bytes (%ju pages)\n",
			    (uintmax_t)phys_avail[indx],
			    (uintmax_t)phys_avail[indx + 1] - 1,
			    (uintmax_t)size, (uintmax_t)size / PAGE_SIZE);
		}
	}

	vm_ksubmap_init(&kmi);

	printf("avail memory = %ju (%ju MB)\n",
	    ptoa((uintmax_t)vm_free_count()),
	    ptoa((uintmax_t)vm_free_count()) / (1024 * 1024));

	bufinit();
	vm_pager_bufferinit();
}

SYSINIT(cpu, SI_SUB_CPU, SI_ORDER_FIRST, cpu_startup, NULL);

int
cpu_idle_wakeup(int cpu)
{

	return (0);
}

void
cpu_idle(int busy)
{

	spinlock_enter();
	if (!busy)
		cpu_idleclock();
	if (!sched_runnable())
//		__asm __volatile(
//		    "fence \n"
//		    "wfi   \n");
	if (!busy)
		cpu_activeclock();
	spinlock_exit();
}

void
cpu_halt(void)
{

	/*
	 * Try to power down using the HSM SBI extension and fall back to a
	 * simple wfi loop.
	 */
	intr_disable();
	for (;;);
		//__asm __volatile("wfi");
	/* NOTREACHED */
}

/*
 * Flush the D-cache for non-DMA I/O so that the I-cache can
 * be made coherent later.
 */
void
cpu_flush_dcache(void *ptr, size_t len)
{

	/* TBD */
}

/* Get current clock frequency for the given CPU ID. */
int
cpu_est_clockrate(int cpu_id, uint64_t *rate)
{
	struct pcpu *pc;

	pc = pcpu_find(cpu_id);
	if (pc == NULL || rate == NULL)
		return (EINVAL);

	if (pc->pc_clock == 0)
		return (EOPNOTSUPP);

	*rate = pc->pc_clock;

	return (0);
}

void
cpu_pcpu_init(struct pcpu *pcpu, int cpuid, size_t size)
{
}

void
spinlock_enter(void)
{
	struct thread *td;
	register_t reg;

	td = curthread;
	if (td->td_md.md_spinlock_count == 0) {
		reg = intr_disable();
		td->td_md.md_spinlock_count = 1;
		td->td_md.md_saved_sstatus_ie = reg;
		critical_enter();
	} else
		td->td_md.md_spinlock_count++;
}

void
spinlock_exit(void)
{
	struct thread *td;
	register_t sstatus_ie;

	td = curthread;
	sstatus_ie = td->td_md.md_saved_sstatus_ie;
	td->td_md.md_spinlock_count--;
	if (td->td_md.md_spinlock_count == 0) {
		critical_exit();
		intr_restore(sstatus_ie);
	}
}

/*
 * Construct a PCB from a trapframe. This is called from kdb_trap() where
 * we want to start a backtrace from the function that caused us to enter
 * the debugger. We have the context in the trapframe, but base the trace
 * on the PCB. The PCB doesn't have to be perfect, as long as it contains
 * enough for a backtrace.
 */
void
makectx(struct trapframe *tf, struct pcb *pcb)
{

	memcpy(pcb->pcb_s, tf->tf_s, sizeof(tf->tf_s));

	pcb->pcb_ra = tf->tf_ra;
	pcb->pcb_sp = tf->tf_sp;
	pcb->pcb_tp = tf->tf_tp;
}

static void
init_proc0(vm_offset_t kstack)
{
	struct pcpu *pcpup;

	pcpup = &__pcpu[0];

	proc_linkup0(&proc0, &thread0);
	thread0.td_kstack = kstack;
	thread0.td_kstack_pages = KSTACK_PAGES;
	thread0.td_pcb = (struct pcb *)(thread0.td_kstack +
	    thread0.td_kstack_pages * PAGE_SIZE) - 1;
	thread0.td_pcb->pcb_fpflags = 0;
	thread0.td_frame = &proc0_tf;
	pcpup->pc_curpcb = thread0.td_pcb;
}

/*
 * Fake up a boot descriptor table.
 */
static void
fake_preload_metadata(struct loongarch_bootparams *rvbp)
{
	static uint32_t fake_preload[48];
	vm_offset_t lastaddr;
	size_t fake_size;

#define PRELOAD_PUSH_VALUE(type, value) do {			\
	*(type *)((char *)fake_preload + fake_size) = (value);	\
	fake_size += sizeof(type);				\
} while (0)

#define PRELOAD_PUSH_STRING(str) do {				\
	uint32_t ssize;						\
	ssize = strlen(str) + 1;				\
	PRELOAD_PUSH_VALUE(uint32_t, ssize);			\
	strcpy(((char *)fake_preload + fake_size), str);	\
	fake_size += ssize;					\
	fake_size = roundup(fake_size, sizeof(u_long));		\
} while (0)

	fake_size = 0;
	lastaddr = (vm_offset_t)&end;

	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_NAME);
	PRELOAD_PUSH_STRING("kernel");
	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_TYPE);
	PRELOAD_PUSH_STRING(preload_kerntype);

	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_ADDR);
	PRELOAD_PUSH_VALUE(uint32_t, sizeof(vm_offset_t));
	PRELOAD_PUSH_VALUE(uint64_t, KERNBASE);

	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_SIZE);
	PRELOAD_PUSH_VALUE(uint32_t, sizeof(size_t));
	PRELOAD_PUSH_VALUE(uint64_t, (size_t)((vm_offset_t)&end - KERNBASE));

	/*
	 * Copy the DTB to KVA space. We are able to dereference the physical
	 * address due to the identity map created in locore.
	 */
	lastaddr = roundup(lastaddr, sizeof(int));
	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_METADATA | MODINFOMD_DTBP);
	PRELOAD_PUSH_VALUE(uint32_t, sizeof(vm_offset_t));
	PRELOAD_PUSH_VALUE(vm_offset_t, lastaddr);
//	dtb_size = fdt_totalsize(rvbp->dtbp_phys);
//	memmove((void *)lastaddr, (const void *)rvbp->dtbp_phys, dtb_size);
//	lastaddr = roundup(lastaddr + dtb_size, sizeof(int));

	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_METADATA | MODINFOMD_KERNEND);
	PRELOAD_PUSH_VALUE(uint32_t, sizeof(vm_offset_t));
	PRELOAD_PUSH_VALUE(vm_offset_t, lastaddr);

	PRELOAD_PUSH_VALUE(uint32_t, MODINFO_METADATA | MODINFOMD_HOWTO);
	PRELOAD_PUSH_VALUE(uint32_t, sizeof(int));
	PRELOAD_PUSH_VALUE(int, RB_VERBOSE);

	/* End marker */
	PRELOAD_PUSH_VALUE(uint32_t, 0);
	PRELOAD_PUSH_VALUE(uint32_t, 0);
	preload_metadata = (caddr_t)fake_preload;

	/* Check if bootloader clobbered part of the kernel with the DTB. */
//	KASSERT(rvbp->dtbp_phys + dtb_size <= rvbp->kern_phys ||
//		rvbp->dtbp_phys >= rvbp->kern_phys + (lastaddr - KERNBASE),
//	    ("FDT (%lx-%lx) and kernel (%lx-%lx) overlap", rvbp->dtbp_phys,
//		rvbp->dtbp_phys + dtb_size, rvbp->kern_phys,
//		rvbp->kern_phys + (lastaddr - KERNBASE)));
	KASSERT(fake_size < sizeof(fake_preload),
	    ("Too many fake_preload items"));

//	if (boothowto & RB_VERBOSE)
//		printf("FDT phys (%lx-%lx), kernel phys (%lx-%lx)\n",
//		    rvbp->dtbp_phys, rvbp->dtbp_phys + dtb_size,
//		    rvbp->kern_phys, rvbp->kern_phys + (lastaddr - KERNBASE));
}

static void
parse_boot_hartid(void)
{
/*
	uint64_t *mdp;

	mdp = (uint64_t *)preload_search_info(preload_kmdp,
	    MODINFO_METADATA | MODINFOMD_BOOT_HARTID);
	if (mdp != NULL && *mdp < UINT32_MAX) {
		boot_hart = (uint32_t)*mdp;
		goto out;
	}

	if (boot_hart == BOOT_HART_INVALID) {
		panic("Boot hart ID was not properly set");
	}

out:
	PCPU_SET(hart, boot_hart);
*/
}

static vm_offset_t
parse_metadata(void)
{
	vm_offset_t lastaddr;
#ifdef DDB
	vm_offset_t ksym_start, ksym_end;
#endif
	char *kern_envp;

	/* Initialize preload_kmdp */
	preload_initkmdp(true);

	/* Read the boot metadata */
	boothowto = MD_FETCH(preload_kmdp, MODINFOMD_HOWTO, int);
	lastaddr = MD_FETCH(preload_kmdp, MODINFOMD_KERNEND, vm_offset_t);
	kern_envp = MD_FETCH(preload_kmdp, MODINFOMD_ENVP, char *);
	if (kern_envp != NULL)
		init_static_kenv(kern_envp, 0);
	else
		init_static_kenv(static_kenv, sizeof(static_kenv));
#ifdef DDB
	ksym_start = MD_FETCH(preload_kmdp, MODINFOMD_SSYM, uintptr_t);
	ksym_end = MD_FETCH(preload_kmdp, MODINFOMD_ESYM, uintptr_t);
	db_fetch_ksymtab(ksym_start, ksym_end, 0);
#endif
	parse_boot_hartid();

	return (lastaddr);
}

void
initloongarch(struct loongarch_bootparams *rvbp)
{
	struct efi_map_header *efihdr;
	struct pcpu *pcpup;
	vm_offset_t lastaddr;
	vm_size_t kernlen;
//	bool first;
	char *env;

	TSRAW(&thread0, TS_ENTER, __func__, NULL);

	/* Set the pcpu data, this is needed by pmap_bootstrap */
	pcpup = &__pcpu[0];
	pcpu_init(pcpup, 0, sizeof(struct pcpu));

	/* Set the pcpu pointer */
	__asm __volatile("move $tp, %0" :: "r"(pcpup));

	PCPU_SET(curthread, &thread0);

	/* Parse the boot metadata. */
	if (rvbp->modulep != 0) {
		preload_metadata = (caddr_t)rvbp->modulep;
	} else {
		fake_preload_metadata(rvbp);
	}
	lastaddr = parse_metadata();

	efihdr = (struct efi_map_header *)preload_search_info(preload_kmdp,
	    MODINFO_METADATA | MODINFOMD_EFI_MAP);
	if (efihdr != NULL) {
		efi_map_add_entries(efihdr);
		efi_map_exclude_entries(efihdr);

		/*
		 * OpenSBI uses the first PMP entry to prevent buggy supervisor
		 * software from overwriting the firmware. However, this
		 * region may not be properly marked as reserved, leading
		 * to an access violation exception whenever the kernel
		 * attempts to write to a page from that region.
		 *
		 * Fix this by excluding first EFI memory map entry
		 * if it is marked as "BootServicesData".
		 */
		//first = true;
		//efi_map_foreach_entry(efihdr, efi_exclude_sbi_pmp_cb, &first);
	}
	/*
	 * Identify CPU/ISA features.
	 */
	identify_cpu(0);

	/* Do basic tuning, hz etc */
	init_param1();

	/* Bootstrap enough of pmap to enter the kernel proper */
	kernlen = (lastaddr - KERNBASE);
	pmap_bootstrap(rvbp->kern_phys, kernlen);

	physmem_init_kernel_globals();

	cninit();

	/*
	 * Dump the boot metadata. We have to wait for cninit() since console
	 * output is required. If it's grossly incorrect the kernel will never
	 * make it this far.
	 */
	if (getenv_is_true("debug.dump_modinfo_at_boot"))
		preload_dump();

	init_proc0(rvbp->kern_stack);

	msgbufinit(msgbufp, msgbufsize);
	mutex_init();
	init_param2(physmem);
	kdb_init();
#ifdef KDB
	if ((boothowto & RB_KDB) != 0)
		kdb_enter(KDB_WHY_BOOTFLAGS, "Boot flags requested debugger");
#endif

	env = kern_getenv("kernelname");
	if (env != NULL)
		strlcpy(kernelname, env, sizeof(kernelname));

	if (boothowto & RB_VERBOSE) {
		if (efihdr != NULL)
			efi_map_print_entries(efihdr);
		physmem_print_tables();
	}

	early_boot = 0;

	if (bootverbose && kstack_pages != KSTACK_PAGES)
		printf("kern.kstack_pages = %d ignored for thread0\n",
		    kstack_pages);

	TSEXIT();
}
