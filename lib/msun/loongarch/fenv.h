/*-
 * Copyright (c) 2004-2005 David Schultz <das@FreeBSD.ORG>
 * Copyright (c) 2015-2016 Ruslan Bukin <br@bsdpad.com>
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

#ifndef	_FENV_H_
#define	_FENV_H_

#include <sys/cdefs.h>
#include <sys/_types.h>

#ifndef	__fenv_static
#define	__fenv_static	static
#endif

typedef	__uint64_t	fenv_t;
typedef	__uint64_t	fexcept_t;


/* Mask for enabling exceptions and for the CAUSE bits.  */
#define ENABLE_MASK 0x0000001FU
#define CAUSE_MASK 0x1F000000U

/* Shift for FE_* flags to get up to the ENABLE bits and the CAUSE bits.  */
#define ENABLE_SHIFT 16
#define CAUSE_SHIFT 8

__BEGIN_DECLS

/* Default floating-point environment */
extern const fenv_t	__fe_dfl_env;
#define	FE_DFL_ENV	(&__fe_dfl_env)

/* LoongArch FPU floating point control register bits.
 *
 * 31-29  -> reserved (read as 0, can not changed by software)
 * 28     -> cause bit for invalid exception
 * 27     -> cause bit for division by zero exception
 * 26     -> cause bit for overflow exception
 * 25     -> cause bit for underflow exception
 * 24     -> cause bit for inexact exception
 * 23-21  -> reserved (read as 0, can not changed by software)
 * 20     -> flag invalid exception
 * 19     -> flag division by zero exception
 * 18     -> flag overflow exception
 * 17     -> flag underflow exception
 * 16     -> flag inexact exception
 *  9-8   -> rounding control
 *  7-5   -> reserved (read as 0, can not changed by software)
 *  4     -> enable exception for invalid exception
 *  3     -> enable exception for division by zero exception
 *  2     -> enable exception for overflow exception
 *  1     -> enable exception for underflow exception
 *  0     -> enable exception for inexact exception
 *
 *
 * Rounding Control:
 * 00 - rounding ties to even (RNE)
 * 01 - rounding toward zero (RZ)
 * 10 - rounding (up) toward plus infinity (RP)
 * 11 - rounding (down) toward minus infinity (RM)
 */

#define _FPU_DEFAULT 0x00000000

#ifdef __loongarch_soft_float
#error only hard float ABI supported
#endif

#define _FPU_RESERVED 0x0
#define _FPU_IEEE 0x1F

/* Masks for interrupts.  */
#define FE_INEXACT      0x01 /* Inexact operation */
#define FE_UNDERFLOW    0x02 /* Underflow */
#define FE_OVERFLOW     0x04 /* Overflow */
#define FE_DIVBYZERO    0x08 /* Division by zero  */
#define FE_INVALID      0x10 /* Invalid operation */
#define	FE_ALL_EXCEPT	(FE_DIVBYZERO | FE_INEXACT | \
			 FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/*
 *  Loongarch Rounding modes
 */
#define	_ROUND_SHIFT	8
#define	FE_TONEAREST	(0x00 << _ROUND_SHIFT)
#define	FE_TOWARDZERO	(0x01 << _ROUND_SHIFT)
#define	FE_DOWNWARD	(0x02 << _ROUND_SHIFT)
#define	FE_UPWARD	(0x03 << _ROUND_SHIFT)
#define	_ROUND_MASK	(FE_TONEAREST | FE_DOWNWARD | \
			 FE_UPWARD | FE_TOWARDZERO)

/* Flush denormalized numbers to zero.  */
#define _FPU_FLUSH_TZ 0x1000000
  
/* Macros for accessing the hardware control word.  */
/*
#define _FPU_GETCW(cw) __asm__ volatile ("movfcsr2gr %0,$r0" : "=r"(cw))
#define _FPU_SETCW(cw) __asm__ volatile ("movgr2fcsr $r0,%0" : : "r"(cw))
*/


#  ifdef __clang__
#    define	__rfs(__fcsr)	__asm __volatile("movfcsr2gr %0, $fcsr0" : "=r" (__fcsr))
#    define	__wfs(__fcsr)	__asm __volatile("movgr2fcsr $fcsr0, %0" :: "r" (__fcsr))
#  else
#    define	__rfs(__fcsr)	__asm __volatile("movfcsr2gr %0, $r0" : "=r" (__fcsr))
#    define	__wfs(__fcsr)	__asm __volatile("movgr2fcsr $r0, %0" :: "r" (__fcsr))
#  endif

# define _FCLASS_SNAN     (1 << 0)
# define _FCLASS_QNAN     (1 << 1)
# define _FCLASS_MINF     (1 << 2)
# define _FCLASS_MNORM    (1 << 3)
# define _FCLASS_MSUBNORM (1 << 4)
# define _FCLASS_MZERO    (1 << 5)
# define _FCLASS_PINF     (1 << 6)
# define _FCLASS_PNORM    (1 << 7)
# define _FCLASS_PSUBNORM (1 << 8)
# define _FCLASS_PZERO    (1 << 9)

# define _FCLASS_ZERO     (_FCLASS_MZERO | _FCLASS_PZERO)
# define _FCLASS_SUBNORM  (_FCLASS_MSUBNORM | _FCLASS_PSUBNORM)
# define _FCLASS_NORM     (_FCLASS_MNORM | _FCLASS_PNORM)
# define _FCLASS_INF      (_FCLASS_MINF | _FCLASS_PINF)
# define _FCLASS_NAN      (_FCLASS_SNAN | _FCLASS_QNAN)

__fenv_static inline int
feclearexcept(int __excepts)
{
  int cw;

  /* Mask out unsupported bits/exceptions.  */
  __excepts &= FE_ALL_EXCEPT;

  /* Read the complete control word.  */
  __rfs(cw);

  /* Clear exception flag bits and cause bits.  If the cause bit is not
     cleared, the next CTC instruction (just below) will re-generate the
     exception.  */

  cw &= ~(__excepts | (__excepts << CAUSE_SHIFT));

  /* Put the new data in effect.  */
  __wfs(cw);

  /* Success.  */
  return 0;
}

__fenv_static inline int
fegetexceptflag(fexcept_t *__flagp, int __excepts)
{
	fexcept_t __fcsr;

	__rfs(__fcsr);
	*__flagp = __fcsr & __excepts;

	return (0);
}

__fenv_static inline int
fesetexceptflag(const fexcept_t *__flagp, int __excepts)
{
	fexcept_t __fcsr;

	__rfs(__fcsr);
	
	/* Make sure the flags we want restored are legal.  */
	__excepts &= FE_ALL_EXCEPT;

	/* Now clear the bits called for, and copy them in from flagp.  Note that
	we ignore all non-flag bits from *flagp, so they don't matter.  */
	__fcsr = (__fcsr & ~__excepts) | (*__flagp & __excepts);

	__wfs(__fcsr);

	/* Success.  */
	return (0);
}

__fenv_static inline int
feraiseexcept(int __excepts)
{

	__wfs(__excepts);
	return (0);
}

__fenv_static inline int
fetestexcept(int __excepts)
{
	fexcept_t __fcsr;

	__rfs(__fcsr);

	return (__fcsr & __excepts);
}

__fenv_static inline int
fegetround(void)
{
	fexcept_t __fcsr;

	__rfs(__fcsr);

	return (__fcsr & _ROUND_MASK);
}

__fenv_static inline int
fesetround(int __round)
{
	fexcept_t __fcsr;

	if (__round & ~_ROUND_MASK)
		return (-1);

	__rfs(__fcsr);
	__fcsr &= ~_ROUND_MASK;
	__fcsr |= __round;
	__wfs(__fcsr);

	return (0);
}

__fenv_static inline int
fegetenv(fenv_t *__envp)
{

	__rfs(*__envp);

	return (0);
}

__fenv_static inline int
feholdexcept(fenv_t *__envp __unused)
{
	fexcept_t __fcsr;

	/* Save the current state.  */
	__rfs(__fcsr);
	*__envp = __fcsr;

	/* Clear all exception enable bits and flags.  */
	__fcsr &= ~FE_ALL_EXCEPT;
	__wfs(__fcsr);

	return (-1);
}

__fenv_static inline int
fesetenv(const fenv_t *__envp)
{

	__wfs(*__envp);

	return (0);
}

__fenv_static inline int
feupdateenv(const fenv_t *__envp)
{
	fexcept_t __fcsr;

	__rfs(__fcsr);
	__wfs(*__envp);
	feraiseexcept(__fcsr & FE_ALL_EXCEPT);

	return (0);
}

#if __BSD_VISIBLE

__fenv_static inline int
feenableexcept(int __mask __unused)
{
  unsigned int new_exc, old_exc;

  /* Get the current control word.  */
  __rfs (new_exc);
 
  old_exc = (new_exc & ENABLE_MASK) << ENABLE_SHIFT;

  __mask &= FE_ALL_EXCEPT;
  
  new_exc |= __mask >> ENABLE_SHIFT;
  __wfs (new_exc);
    
  return old_exc;
	/* No exception traps. */
}

__fenv_static inline int
fedisableexcept(int __mask __unused)
{
  unsigned int new_exc, old_exc;
 
  /* Get the current control word.  */
  __rfs (new_exc);

  old_exc = (new_exc & ENABLE_MASK) << ENABLE_SHIFT;
  
  __mask &= FE_ALL_EXCEPT;
  
  new_exc &= ~(__mask >> ENABLE_SHIFT);
  __wfs (new_exc);

  return old_exc;

	/* No exception traps. */
}

static inline int
fegetexcept(void)
{
  unsigned int exc;

  /* Get the current control word.  */
  __rfs (exc);

  return (exc & ENABLE_MASK) << ENABLE_SHIFT;
        /* No exception traps. */
}

#endif /* __BSD_VISIBLE */

__END_DECLS

#endif	/* !_FENV_H_ */
