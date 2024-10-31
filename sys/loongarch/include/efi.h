/*-
 * This file is in the public domain since it's just boilerplate.
 */

#ifndef __LOONGARCH_INCLUDE_EFI_H_
#define __LOONGARCH_INCLUDE_EFI_H_

#define	EFIABI_ATTR

#ifdef _KERNEL
#define ARCH_MAY_USE_EFI

#define	EFI_TIME_LOCK()
#define	EFI_TIME_UNLOCK()
#define	EFI_TIME_OWNED()

#define	EFI_RT_HANDLE_FAULTS_DEFAULT	0
#endif

struct efirt_callinfo {
	const char	*ec_name;
	register_t	ec_efi_status;
	register_t	ec_fptr;
	register_t	ec_argcnt;
	register_t	ec_arg1;
	register_t	ec_arg2;
	register_t	ec_arg3;
	register_t	ec_arg4;
	register_t	ec_arg5;
};

#endif /* __LOONGARCH_INCLUDE_EFI_H_ */
