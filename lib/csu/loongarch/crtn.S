#include <machine/asm.h>
__FBSDID("$FreeBSD$");

#ifndef __clang__
	.gnu_attribute 4, 0
#endif
	.section .init,"ax",%progbits
#if defined(__ABICALLS__)
	REG_L	fp, sp, CALLFRAME_GP
#endif
	REG_L	ra, sp, CALLFRAME_RA
	jr	ra
	PTR_ADDI sp, sp, CALLFRAME_SIZ

	.section .fini,"ax",%progbits
#if defined(__ABICALLS__)
	REG_L	fp, sp, CALLFRAME_GP
#endif
	REG_L	ra, sp, CALLFRAME_RA
	jr	ra
	PTR_ADDI sp, sp, CALLFRAME_SIZ
