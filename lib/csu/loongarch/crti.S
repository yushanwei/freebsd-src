#include <machine/asm.h>
__FBSDID("$FreeBSD$");

#ifndef __clang__
	.gnu_attribute 4, 0
#endif
	.section .init,"ax",%progbits
	.align 4
	.globl	_init
	.type	_init,%function
_init:
	PTR_ADDI sp, sp, -CALLFRAME_SIZ
	REG_S	ra, sp, CALLFRAME_RA
#if defined(__ABICALLS__)
	SETUP_GP64(CALLFRAME_GP, _init)
#else /* __ABICALLS__ */
	REG_S	fp, sp,CALLFRAME_GP
#endif

	.section .fini,"ax",%progbits
	.align 4
	.globl	_fini
	.type	_fini,%function
_fini:
	PTR_ADDI sp, sp, -CALLFRAME_SIZ
	REG_S	ra, sp, CALLFRAME_RA
#if defined(__ABICALLS__)
	SETUP_GP64(CALLFRAME_GP, _fini)
#else /* __ABICALLS__ */
	REG_S	fp, sp, CALLFRAME_GP
#endif
