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

#include <sys/_types.h>

#ifndef	__fenv_static
#define	__fenv_static	static
#endif

typedef	__uint64_t	fenv_t;
typedef	__uint64_t	fexcept_t;

/* Exception flags */
#define	FE_INVALID	0x100000
#define	FE_DIVBYZERO	0x080000
#define	FE_OVERFLOW	 0x040000
#define	FE_UNDERFLOW	0x020000
#define	FE_INEXACT	0x010000
#define	FE_ALL_EXCEPT	(FE_DIVBYZERO | FE_INEXACT | \
			 FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)


/*
 * Loongarch Rounding mode
 */
#define _ROUND_SHIFT 8
#define FE_TONEAREST (0x0 << _ROUND_SHIFT)
#define FE_TOWARDZERO (0x1 << _ROUND_SHIFT)
#define FE_UPWARD (0x2 << _ROUND_SHIFT)
#define FE_DOWNWARD (0x3 << _ROUND_SHIFT)

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

#ifdef __loongarch_soft_float

#define _FPU_RESERVED 0xffffffff
#define _FPU_DEFAULT 0x00000000
typedef unsigned int fpu_control_t;
#define _FPU_GETCW(cw) (cw) = 0
#define _FPU_SETCW(cw) (void) (cw)
extern fpu_control_t __fpu_control;

#else /* __loongarch_soft_float */

/* Masks for interrupts.  */
#define _FPU_MASK_V 0x10 /* Invalid operation */
#define _FPU_MASK_Z 0x08 /* Division by zero  */
#define _FPU_MASK_O 0x04 /* Overflow */
#define _FPU_MASK_U 0x02 /* Underflow */
#define _FPU_MASK_I 0x01 /* Inexact operation */

/* Flush denormalized numbers to zero.  */
#define _FPU_FLUSH_TZ 0x1000000

/* Rounding control.  */
#define _FPU_RC_NEAREST 0x000 /* RECOMMENDED */
#define _FPU_RC_ZERO 0x100
#define _FPU_RC_UP 0x200
#define _FPU_RC_DOWN 0x300
/* Mask for rounding control.  */
#define _FPU_RC_MASK 0x300

#define _FPU_RESERVED 0x0

#define _FPU_DEFAULT 0x0
#define _FPU_IEEE 0x1F
/* Type of the control word.  */
typedef unsigned int fpu_control_t __attribute__ ((__mode__ (__SI__)));

/* Macros for accessing the hardware control word.  */
extern fpu_control_t __loongarch_fpu_getcw (void) __THROW;
extern void __loongarch_fpu_setcw (fpu_control_t) __THROW;
#define _FPU_GETCW(cw) __asm__ volatile ("movfcsr2gr %0,$r0" : "=r"(cw))
#define _FPU_SETCW(cw) __asm__ volatile ("movgr2fcsr $r0,%0" : : "r"(cw))

/* Default control word set at startup.  */
extern fpu_control_t __fpu_control;


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
feclearexcept(int excepts)
{
  int cw;

  /* Mask out unsupported bits/exceptions.  */
  excepts &= FE_ALL_EXCEPT;

  /* Read the complete control word.  */
  _FPU_GETCW (cw);

  /* Clear exception flag bits and cause bits.  If the cause bit is not
     cleared, the next CTC instruction (just below) will re-generate the
     exception.  */

  cw &= ~(excepts | (excepts << CAUSE_SHIFT));

  /* Put the new data in effect.  */
  _FPU_SETCW (cw);

  /* Success.  */
  return 0;
}

__fenv_static inline int
fegetexceptflag(fexcept_t *flagp, int excepts)
{
   fpu_control_t temp;

  /* Get the current exceptions.  */
  _FPU_GETCW (temp);

  /* We only save the relevant bits here.  In particular, care has to be
     taken with the CAUSE bits, as an inadvertent restore later on could
     generate unexpected exceptions.  */

  *flagp = temp & excepts & FE_ALL_EXCEPT;

  /* Success.  */
  return 0;
}

__fenv_static inline int
fesetexceptflag(const fexcept_t *flagp, int excepts)
{
   fpu_control_t temp;

  /* Get the current exceptions.  */
  _FPU_GETCW (temp);

  /* Make sure the flags we want restored are legal.  */
  excepts &= FE_ALL_EXCEPT;

  /* Now clear the bits called for, and copy them in from flagp.  Note that
     we ignore all non-flag bits from *flagp, so they don't matter.  */
  temp = (temp & ~excepts) | (*flagp & excepts);

  _FPU_SETCW (temp);

  /* Success.  */
  return 0;
}

__fenv_static inline int
feraiseexcept(int excepts)
{
  const float fp_zero = 0.0f;
  const float fp_one = 1.0f;
  const float fp_max = FLT_MAX;
  const float fp_min = FLT_MIN;
  const float fp_1e32 = 1.0e32f;
  const float fp_two = 2.0f;
  const float fp_three = 3.0f;

  /* Raise exceptions represented by EXPECTS.  But we must raise only
     one signal at a time.  It is important that if the overflow/underflow
     exception and the inexact exception are given at the same time,
     the overflow/underflow exception follows the inexact exception.  */

  /* First: invalid exception.  */
  if (FE_INVALID & excepts)
    __asm__ __volatile__("fdiv.s $f0,%0,%0\n\t"
             :
             : "f"(fp_zero)
             : "$f0");

  /* Next: division by zero.  */
  if (FE_DIVBYZERO & excepts)
    __asm__ __volatile__("fdiv.s $f0,%0,%1\n\t"
             :
             : "f"(fp_one), "f"(fp_zero)
             : "$f0");

  /* Next: overflow.  */
  if (FE_OVERFLOW & excepts)
    /* There's no way to raise overflow without also raising inexact.  */
    __asm__ __volatile__("fadd.s $f0,%0,%1\n\t"
             :
             : "f"(fp_max), "f"(fp_1e32)
             : "$f0");

  /* Next: underflow.  */
  if (FE_UNDERFLOW & excepts)
    __asm__ __volatile__("fdiv.s $f0,%0,%1\n\t"
             :
             : "f"(fp_min), "f"(fp_three)
             : "$f0");

  /* Last: inexact.  */
  if (FE_INEXACT & excepts)
    __asm__ __volatile__("fdiv.s $f0, %0, %1\n\t"
             :
             : "f"(fp_two), "f"(fp_three)
             : "$f0");

  /* Success.  */
  return 0;
}

__fenv_static inline int
fetestexcept(int excepts)
{
  int cw;

  /* Get current control word.  */
  _FPU_GETCW (cw);

  return cw & excepts & FE_ALL_EXCEPT;
}

__fenv_static inline int
fegetround(void)
{
  int cw;

  /* Get control word.  */
  _FPU_GETCW (cw);

  return cw & _FPU_RC_MASK;
}

__fenv_static inline int
fesetround(int round)
{
  fpu_control_t cw;

  if ((round & ~_FPU_RC_MASK) != 0)
    /* ROUND is no valid rounding mode.  */
    return 1;

  /* Get current state.  */
  _FPU_GETCW (cw);

  /* Set rounding bits.  */
  cw &= ~_FPU_RC_MASK;
  cw |= round;
  /* Set new state.  */
  _FPU_SETCW (cw);

  return 0;
}

__fenv_static inline int
fegetenv(fenv_t *envp)
{
  _FPU_GETCW (*envp);

  /* Success.  */
  return 0;
}

__fenv_static inline int
feholdexcept(fenv_t *envp)
{
  fpu_control_t cw;

  /* Save the current state.  */
  _FPU_GETCW (cw);
  envp->__fp_control_register = cw;

  /* Clear all exception enable bits and flags.  */
  cw &= ~(_FPU_MASK_V | _FPU_MASK_Z | _FPU_MASK_O | _FPU_MASK_U | _FPU_MASK_I
      | FE_ALL_EXCEPT);
  _FPU_SETCW (cw);

  return 0;
}

__fenv_static inline int
fesetenv(const fenv_t *envp)
{
  fpu_control_t cw;

  /* Read first current state to flush fpu pipeline.  */
  _FPU_GETCW (cw);

  if (envp == FE_DFL_ENV)
    _FPU_SETCW (_FPU_DEFAULT);
  else if (envp == FE_NOMASK_ENV)
    _FPU_SETCW (_FPU_IEEE);
  else
    _FPU_SETCW (envp->__fp_control_register);

  /* Success.  */
  return 0;
}

__fenv_static inline int
feupdateenv(const fenv_t *envp)
{
  int temp;

  /* Save current exceptions.  */
  _FPU_GETCW (temp);
  temp &= FE_ALL_EXCEPT;

  /* Install new environment.  */
  __fesetenv (envp);

  /* Raise the saved exception.  Incidentally for us the implementation
     defined format of the values in objects of type fexcept_t is the
     same as the ones specified using the FE_* constants.  */
  __feraiseexcept (temp);

  /* Success.  */
  return 0;
}

#if __BSD_VISIBLE

/* We currently provide no external definitions of the functions below. */

__fenv_static inline int
feenableexcept(int mask)
{
  unsigned int new_exc, old_exc;

  /* Get the current control word.  */
  _FPU_GETCW (new_exc);

  old_exc = (new_exc & ENABLE_MASK) << ENABLE_SHIFT;

  excepts &= FE_ALL_EXCEPT;

  new_exc |= excepts >> ENABLE_SHIFT;
  _FPU_SETCW (new_exc);

  return old_exc;
}

__fenv_static inline int
fedisableexcept(int mask)
{
  unsigned int new_exc, old_exc;

  /* Get the current control word.  */
  _FPU_GETCW (new_exc);

  old_exc = (new_exc & ENABLE_MASK) << ENABLE_SHIFT;

  excepts &= FE_ALL_EXCEPT;

  new_exc &= ~(excepts >> ENABLE_SHIFT);
  _FPU_SETCW (new_exc);

  return old_exc;
}

__fenv_static inline int
fegetexcept(void)
{
  unsigned int exc;

  /* Get the current control word.  */
  _FPU_GETCW (exc);

  return (exc & ENABLE_MASK) << ENABLE_SHIFT;
}

#endif /* __BSD_VISIBLE */

#endif /* __loongarch_soft_float */


__END_DECLS

#endif /* !_FENV_H_ */

