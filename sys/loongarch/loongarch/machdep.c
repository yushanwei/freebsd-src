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

#include "opt_platform.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/boot.h>
#include <sys/buf.h>
#include <sys/bus.h>
#include <sys/cons.h>
#include <sys/cpu.h>
#include <sys/devmap.h>
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

struct pcpu __pcpu[MAXCPU];

static struct trapframe proc0_tf;

int early_boot = 1;
int cold = 1;

#define	DTB_SIZE_MAX	(1024 * 1024)

struct kva_md_info kmi;

int64_t dcache_line_size;	/* The minimum D cache line size */
int64_t icache_line_size;	/* The minimum I cache line size */
int64_t idcache_line_size;	/* The minimum cache line size */

#define BOOT_HART_INVALID	0xffffffff
uint32_t boot_hart = BOOT_HART_INVALID;	/* The hart we booted on. */

cpuset_t all_harts;

extern int *end;
extern int *initstack_end;

struct pcpu *pcpup;


static char static_kenv[PAGE_SIZE];

static void
cpu_startup(void *dummy)
{

	printcpuinfo(0);

	vm_ksubmap_init(&kmi);

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
		__asm __volatile(
		    "fence \n"
		    "wfi   \n");
	if (!busy)
		cpu_activeclock();
	spinlock_exit();
}

void
cpu_halt(void)
{
	panic("cpu_halt");
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

	panic("cpu_est_clockrate");
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

	pcb->pcb_ra = tf->tf_sepc;
	pcb->pcb_sp = tf->tf_sp;
	pcb->pcb_gp = tf->tf_gp;
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

static void
cache_setup(void)
{

	/* TODO */
}

void
initloongarch(struct loongarch_bootparams *rvbp)
{
        vm_offset_t lastaddr;
        vm_size_t kernlen;
        caddr_t kmdp;

        /* Set the module data location */
        lastaddr = fake_preload_metadata(rvbp);

        /* Find the kernel address */
        kmdp = preload_search_by_type("elf kernel");
        if (kmdp == NULL)
                kmdp = preload_search_by_type("elf64 kernel");

        boothowto = 0;

        kern_envp = NULL;
                        
        /* Load the physical memory ranges */
        physmap_idx = 0;
                        
        /* Set the pcpu data, this is needed by pmap_bootstrap */
        pcpup = &__pcpu[0];
        pcpu_init(pcpup, 0, sizeof(struct pcpu));

        /* Set the pcpu pointer */
#if 0
        /* SMP TODO: try re-use gp for pcpu pointer */
        __asm __volatile(
            "mv gp, %0" :: "r"(pcpup));
#endif
        PCPU_SET(curthread, &thread0);

        /* Do basic tuning, hz etc */
        init_param1();

        cache_setup();

        /* Bootstrap enough of pmap  to enter the kernel proper */
        kernlen = (lastaddr - KERNBASE);
        pmap_bootstrap(rvbp->kern_phys, kernlen);

        cninit();

        init_proc0(rvbp->kern_stack);

        msgbufinit(msgbufp, msgbufsize);
        mutex_init();
        init_param2(physmem);
        kdb_init();

        early_boot = 0;

}
