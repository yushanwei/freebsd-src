#ifndef _MACHINE_TRAP_H_
#define _MACHINE_TRAP_H_
#if __loongarch_grlen == 32
#define RSIZE   4
#elif __loongarch_grlen == 64
#define RSIZE   8
#endif

#define GP_REG_CONTEXT_SIZE	32 * RSIZE	// general purpose regis ters
#define FP_REG_CONTEXT_SIZE	34 * RSIZE	// Floating-point regist ers
#define CSR_REG_CONTEXT_SIZE	9 * RSIZE	// CSR registers

#endif /* _MACHINE_TRAP_H_ */
