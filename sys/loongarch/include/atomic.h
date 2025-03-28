/*
 * Atomic operations.
 *
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */
#ifndef _MACHINE_ATOMIC_H_
#define _MACHINE_ATOMIC_H_
#include <sys/atomic_common.h>
/*
 * Hint encoding:
 *
 * Bit4: ordering or completion (0: completion, 1: ordering)
 * Bit3: barrier for previous read (0: true, 1: false)
 * Bit2: barrier for previous write (0: true, 1: false)
 * Bit1: barrier for succeeding read (0: true, 1: false)
 * Bit0: barrier for succeeding write (0: true, 1: false)
 *
 * Hint 0x700: barrier for "read after read" from the same address
 */

#define DBAR(hint) __asm__ __volatile__("dbar %0 " : : "I"(hint) : "memory")

#define crwrw		0b00000
#define cr_r_		0b00101
#define c_w_w		0b01010

#define orwrw		0b10000
#define or_r_		0b10101
#define o_w_w		0b11010

#define orw_w		0b10010
#define or_rw		0b10100

#define c_sync()	DBAR(crwrw)
#define c_rsync()	DBAR(cr_r_)
#define c_wsync()	DBAR(c_w_w)

#define o_sync()	DBAR(orwrw)
#define o_rsync()	DBAR(or_r_)
#define o_wsync()	DBAR(o_w_w)

#define ldacq_mb()	DBAR(or_rw)
#define strel_mb()	DBAR(orw_w)

#define mb()		c_sync()
#define rmb()		c_rsync()
#define wmb()		c_wsync()
#define iob()		c_sync()
#define wbflush()	c_sync()

#define __smp_mb()	o_sync()
#define __smp_rmb()	o_rsync()
#define __smp_wmb()	o_wsync()

#ifdef SMP
#define __WEAK_LLSC_MB		"	dbar 0x700	\n"
#else
#define __WEAK_LLSC_MB		"			\n"
#endif

#	if __loongarch_grlen  == 32
#define __LL		"ll.w	"
#define __SC		"sc.w	"
#define __AMADD		"amadd.w	"
#define __AMAND_DB	"amand_db.w	"
#define __AMOR_DB	"amor_db.w	"
#define __AMXOR_DB	"amxor_db.w	"
#elif __loongarch_grlen  == 64
#define __LL		"ll.d	"
#define __SC		"sc.d	"
#define __AMADD		"amadd.d	"
#define __AMAND_DB	"amand_db.d	"
#define __AMOR_DB	"amor_db.d	"
#define __AMXOR_DB	"amxor_db.d	"
#endif

static __inline int atomic_cmpset_8(__volatile uint8_t *, uint8_t, uint8_t);
static __inline int atomic_fcmpset_8(__volatile uint8_t *, uint8_t *, uint8_t);
static __inline int atomic_cmpset_16(__volatile uint16_t *, uint16_t, uint16_t);
static __inline int atomic_fcmpset_16(__volatile uint16_t *, uint16_t *, uint16_t);

#define ATOMIC_ACQ_REL(NAME, WIDTH)                                     \
static __inline  void                                                   \
atomic_##NAME##_acq_##WIDTH(__volatile uint##WIDTH##_t *p, uint##WIDTH##_t v)\
{                                                                       \
        atomic_##NAME##_##WIDTH(p, v);                                  \
        ldacq_mb();                                                        \
}                                                                       \
                                                                        \
static __inline  void                                                   \
atomic_##NAME##_rel_##WIDTH(__volatile uint##WIDTH##_t *p, uint##WIDTH##_t v)\
{                                                                       \
        strel_mb();                                                        \
        atomic_##NAME##_##WIDTH(p, v);                                  \
}
 
#define ATOMIC_CMPSET_ACQ_REL(WIDTH)                                    \
static __inline  int                                                    \
atomic_cmpset_acq_##WIDTH(__volatile uint##WIDTH##_t *p,                \
    uint##WIDTH##_t cmpval, uint##WIDTH##_t newval)                     \
{                                                                       \
        int retval;                                                     \
                                                                        \
        retval = atomic_cmpset_##WIDTH(p, cmpval, newval);              \
        ldacq_mb();                                                     \
        return (retval);                                                \
}                                                                       \
                                                                        \
static __inline  int                                                    \
atomic_cmpset_rel_##WIDTH(__volatile uint##WIDTH##_t *p,                \
    uint##WIDTH##_t cmpval, uint##WIDTH##_t newval)                     \
{                                                                       \
        strel_mb();                                                     \
        return (atomic_cmpset_##WIDTH(p, cmpval, newval));              \
}
 
#define ATOMIC_FCMPSET_ACQ_REL(WIDTH)                                   \
static __inline  int                                                    \
atomic_fcmpset_acq_##WIDTH(__volatile uint##WIDTH##_t *p,               \
    uint##WIDTH##_t *cmpval, uint##WIDTH##_t newval)                    \
{                                                                       \
        int retval;                                                     \
                                                                        \
        retval = atomic_fcmpset_##WIDTH(p, cmpval, newval);             \
        ldacq_mb();							\
        return (retval);                                                \
}                                                                       \
                                                                        \
static __inline  int                                                    \
atomic_fcmpset_rel_##WIDTH(__volatile uint##WIDTH##_t *p,               \
    uint##WIDTH##_t *cmpval, uint##WIDTH##_t newval)                    \
{                                                                       \
        strel_mb();							\
        return (atomic_fcmpset_##WIDTH(p, cmpval, newval));             \
}

ATOMIC_CMPSET_ACQ_REL(8);
ATOMIC_FCMPSET_ACQ_REL(8);
ATOMIC_CMPSET_ACQ_REL(16);
ATOMIC_FCMPSET_ACQ_REL(16);

#define atomic_cmpset_char              atomic_cmpset_8
#define atomic_cmpset_acq_char          atomic_cmpset_acq_8
#define atomic_cmpset_rel_char          atomic_cmpset_rel_8
#define atomic_fcmpset_char             atomic_fcmpset_8
#define atomic_fcmpset_acq_char         atomic_fcmpset_acq_8
#define atomic_fcmpset_rel_char         atomic_fcmpset_rel_8

#define atomic_cmpset_short             atomic_cmpset_16
#define atomic_cmpset_acq_short         atomic_cmpset_acq_16
#define atomic_cmpset_rel_short         atomic_cmpset_rel_16
#define atomic_fcmpset_short            atomic_fcmpset_16
#define atomic_fcmpset_acq_short        atomic_fcmpset_acq_16
#define atomic_fcmpset_rel_short        atomic_fcmpset_rel_16

static __inline void
atomic_add_32(volatile uint32_t *p, uint32_t val)
{
        __asm __volatile("amadd.w $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (val)
                        : "memory");
}

static __inline void
atomic_subtract_32(volatile uint32_t *p, uint32_t val)
{
        __asm __volatile("amadd.w $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (-val)
                        : "memory");
}

static __inline void
atomic_set_32(volatile uint32_t *p, uint32_t val)
{
        __asm __volatile("amor.w $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (val)
                        : "memory");
}


static __inline void
atomic_clear_32(volatile uint32_t *p, uint32_t val)
{
        __asm __volatile("amand.w $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (val)
                        : "memory");
}

static __inline int
atomic_cmpset_32(volatile uint32_t *p, uint32_t cmpval, uint32_t newval)
{
	uint32_t tmp;

        __asm __volatile(
		"1:	ll.w	%0, %2		\n"
		"	bne	%0, %z3, 2f	\n"
		"	move	$t0, %z4	\n"
		"	sc.w	$t0, %1		\n"
		"	beqz	$t0, 1b		\n"
		"2:				\n"
		__WEAK_LLSC_MB
		: "=&r" (tmp), "=ZB"(*p)
		: "ZB"(*p), "Jr" (cmpval), "Jr" (newval)
		: "t0", "memory");
/*
		"1:	ll.w	%[tmp], %[p]	\n"
		"	bne	%[tmp], %z[cmpval], 2f	\n"
		"	move	$t0, %z[newval]	\n"
		"	sc.w	$t0, %[p]	\n"
		"	beqz	$t0, 1b		\n"
		"2:	dbar	0x700		\n"
		: "=&r" (tmp), "=ZB"(*p)
		: "ZB"(*p), "Jr" (cmpval), "Jr" (newval)
		: "t0", "memory");
*/
        return (tmp);
}
static __inline int
atomic_fcmpset_32(volatile uint32_t *p, uint32_t *cmpval, uint32_t newval)
{
	__typeof(newval) tmp;

        __asm __volatile(
		"1:	ll.w	%0, %2		\n"
		"	bne	%0, %z3, 2f	\n"
		"	move	$t0, %z4	\n"
		"	sc.w	$t0, %1		\n"
		"	beqz	$t0, 1b		\n"
		"2:				\n"
		__WEAK_LLSC_MB
		: "=&r" (tmp), "=ZB"(*p)
		: "ZB"(*p), "Jr" (*cmpval), "Jr" (newval)
		: "t0", "memory");

        return (tmp);
}
static __inline uint32_t
atomic_fetchadd_32(volatile uint32_t *p, uint32_t val)
{
        uint32_t ret;

        __asm __volatile("amadd.w %0, %2, %1"
                        : "=&r" (ret), "+ZB" (*p)
                        : "r" (val)
                        : "memory");

        return (ret);
}
static __inline uint32_t
atomic_readandclear_32(volatile uint32_t *p)
{
        uint32_t ret;
        uint32_t val;

        val = 0;

        __asm __volatile("amswap.w %0, %2, %1"
                        : "=&r"(ret), "+ZB" (*p)
                        : "r" (val)
                        : "memory");

        return (ret);
}

#define atomic_add_int          atomic_add_32
#define atomic_clear_int        atomic_clear_32
#define atomic_cmpset_int       atomic_cmpset_32
#define atomic_fcmpset_int      atomic_fcmpset_32
#define atomic_fetchadd_int     atomic_fetchadd_32
#define atomic_readandclear_int atomic_readandclear_32
#define atomic_set_int          atomic_set_32
#define atomic_subtract_int     atomic_subtract_32

ATOMIC_ACQ_REL(set, 32)
ATOMIC_ACQ_REL(clear, 32)
ATOMIC_ACQ_REL(add, 32)
ATOMIC_ACQ_REL(subtract, 32)

ATOMIC_CMPSET_ACQ_REL(32);
ATOMIC_FCMPSET_ACQ_REL(32);

static __inline uint32_t
atomic_load_acq_32(const volatile uint32_t *p)
{
        uint32_t ret;
        ret = *p;
        ldacq_mb();
        return (ret);
}

static __inline void
atomic_store_rel_32(volatile uint32_t *p, uint32_t val)
{
        strel_mb();
        *p = val;
}
#define atomic_add_acq_int      atomic_add_acq_32
#define atomic_clear_acq_int    atomic_clear_acq_32
#define atomic_cmpset_acq_int   atomic_cmpset_acq_32
#define atomic_fcmpset_acq_int  atomic_fcmpset_acq_32
#define atomic_load_acq_int     atomic_load_acq_32
#define atomic_set_acq_int      atomic_set_acq_32
#define atomic_subtract_acq_int atomic_subtract_acq_32

#define atomic_add_rel_int      atomic_add_rel_32
#define atomic_clear_rel_int    atomic_clear_rel_32
#define atomic_cmpset_rel_int   atomic_cmpset_rel_32
#define atomic_fcmpset_rel_int  atomic_fcmpset_rel_32
#define atomic_set_rel_int      atomic_set_rel_32
#define atomic_subtract_rel_int atomic_subtract_rel_32
#define atomic_store_rel_int    atomic_store_rel_32

static __inline void
atomic_add_64(volatile uint64_t *p, uint64_t val)
{

        __asm __volatile("amadd.d $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (val)
                        : "memory");
}

static __inline void
atomic_subtract_64(volatile uint64_t *p, uint64_t val)
{

        __asm __volatile("amadd.d $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (-val)
                        : "memory");
}

static __inline void
atomic_set_64(volatile uint64_t *p, uint64_t val)
{

        __asm __volatile("amor.d $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (val)
                        : "memory");
}
static __inline void
atomic_clear_64(volatile uint64_t *p, uint64_t val)
{

        __asm __volatile("amand.d $zero, %1, %0"
                        : "+ZB" (*p)
                        : "r" (~val)
                        : "memory");
}

static __inline int
atomic_cmpset_64(volatile uint64_t *p, uint64_t cmpval, uint64_t newval)
{
        uint64_t tmp;
        int res;

        res = 0;
        __asm __volatile(
                "0:"
                        "li.d %1, 1\n" /* Preset to fail */
                        "ll.d %0, %2\n"
                        "bne  %0, %z3, 1f\n"
                        "sc.d %1, %2\n"
                        "bnez %1, 0b\n"
                "1:"
                        : "=&r" (tmp), "=&r" (res), "+ZB" (*p)
                        : "rJ" (cmpval), "rJ" (newval)
                        : "memory");
        return (!res);
}

static __inline int
atomic_fcmpset_64(volatile uint64_t *p, uint64_t *cmpval, uint64_t newval)
{
        uint64_t tmp;
        int res;

        res = 0;
        __asm __volatile(
                "0:"
                        "li.d %1, 1\n"          /* Preset to fail */
                        "ll.d %0, %2\n"         /* Load old value */
                        "bne  %0, %z4, 1f\n"    /* Compare */
                        "sc.d %1, %2\n"    /* Try to store new value */
                        "b 2f\n"
                "1:"
                        "st.d   %0, %3\n"         /* Save old value */
                "2:"
                        : "=&r" (tmp), "=&r" (res), "+ZB" (*p), "+ZB" (*cmpval)
                        : "rJ" (*cmpval), "rJ" (newval)
                        : "memory");

        return (!res);
}

static __inline uint64_t
atomic_fetchadd_64(volatile uint64_t *p, uint64_t val)
{
        uint64_t ret;

        __asm __volatile("amadd.d %0, %2, %1"
                        : "=&r" (ret), "+ZB" (*p)
                        : "r" (val)
                        : "memory");

        return (ret);
}


static __inline uint64_t
atomic_readandclear_64(volatile uint64_t *p)
{
        uint64_t ret;
        uint64_t val;

        val = 0;

        __asm __volatile("amswap.d %0, %2, %1"
                        : "=&r"(ret), "+ZB" (*p)
                        : "r" (val)
                        : "memory");

        return (ret);
}

static __inline uint32_t
atomic_swap_32(volatile uint32_t *p, uint32_t val)
{
        uint32_t old;

        __asm __volatile("amswap.w %0, %2, %1"
                        : "=&r"(old), "+ZB" (*p)
                        : "r" (val)
                        : "memory");

        return (old);
}

static __inline uint64_t
atomic_swap_64(volatile uint64_t *p, uint64_t val)
{
        uint64_t old;

        __asm __volatile("amswap.d %0, %2, %1"
                        : "=&r"(old), "+ZB" (*p)
                        : "r" (val)
                        : "memory");

        return (old);
}

#define atomic_swap_int                 atomic_swap_32

#define atomic_add_long                 atomic_add_64
#define atomic_clear_long               atomic_clear_64
#define atomic_cmpset_long              atomic_cmpset_64
#define atomic_fcmpset_long             atomic_fcmpset_64
#define atomic_fetchadd_long            atomic_fetchadd_64
#define atomic_readandclear_long        atomic_readandclear_64
#define atomic_set_long                 atomic_set_64
#define atomic_subtract_long            atomic_subtract_64
#define atomic_swap_long                atomic_swap_64

#define atomic_add_ptr                  atomic_add_64
#define atomic_clear_ptr                atomic_clear_64
#define atomic_cmpset_ptr               atomic_cmpset_64
#define atomic_fcmpset_ptr              atomic_fcmpset_64
#define atomic_fetchadd_ptr             atomic_fetchadd_64
#define atomic_readandclear_ptr         atomic_readandclear_64
#define atomic_set_ptr                  atomic_set_64
#define atomic_subtract_ptr             atomic_subtract_64
#define atomic_swap_ptr                 atomic_swap_64

ATOMIC_ACQ_REL(set, 64)
ATOMIC_ACQ_REL(clear, 64)
ATOMIC_ACQ_REL(add, 64)
ATOMIC_ACQ_REL(subtract, 64)

ATOMIC_CMPSET_ACQ_REL(64);
ATOMIC_FCMPSET_ACQ_REL(64);

static __inline uint64_t
atomic_load_acq_64(const volatile uint64_t *p)
{
        uint64_t ret;

        ret = *p;
        ldacq_mb();
        return (ret);
}

static __inline void
atomic_store_rel_64(volatile uint64_t *p, uint64_t val)
{

        strel_mb();
        *p = val;
}

#define atomic_add_acq_long             atomic_add_acq_64
#define atomic_clear_acq_long           atomic_clear_acq_64
#define atomic_cmpset_acq_long          atomic_cmpset_acq_64
#define atomic_fcmpset_acq_long         atomic_fcmpset_acq_64
#define atomic_load_acq_long            atomic_load_acq_64
#define atomic_set_acq_long             atomic_set_acq_64
#define atomic_subtract_acq_long        atomic_subtract_acq_64

#define atomic_add_acq_ptr              atomic_add_acq_64
#define atomic_clear_acq_ptr            atomic_clear_acq_64
#define atomic_cmpset_acq_ptr           atomic_cmpset_acq_64
#define atomic_fcmpset_acq_ptr          atomic_fcmpset_acq_64
#define atomic_load_acq_ptr             atomic_load_acq_64
#define atomic_set_acq_ptr              atomic_set_acq_64
#define atomic_subtract_acq_ptr         atomic_subtract_acq_64

#undef ATOMIC_ACQ_REL

static __inline void
atomic_thread_fence_acq(void)
{

        ldacq_mb();
}

static __inline void
atomic_thread_fence_rel(void)
{

        strel_mb();
}

static __inline void
atomic_thread_fence_acq_rel(void)
{

        __smp_mb();
}
static __inline void
atomic_thread_fence_seq_cst(void)
{

        mb();
}

#define atomic_add_rel_long             atomic_add_rel_64
#define atomic_clear_rel_long           atomic_clear_rel_64

#define atomic_add_rel_long             atomic_add_rel_64
#define atomic_clear_rel_long           atomic_clear_rel_64
#define atomic_cmpset_rel_long          atomic_cmpset_rel_64
#define atomic_fcmpset_rel_long         atomic_fcmpset_rel_64
#define atomic_set_rel_long             atomic_set_rel_64
#define atomic_subtract_rel_long        atomic_subtract_rel_64
#define atomic_store_rel_long           atomic_store_rel_64

#define atomic_add_rel_ptr              atomic_add_rel_64
#define atomic_clear_rel_ptr            atomic_clear_rel_64
#define atomic_cmpset_rel_ptr           atomic_cmpset_rel_64
#define atomic_fcmpset_rel_ptr          atomic_fcmpset_rel_64
#define atomic_set_rel_ptr              atomic_set_rel_64
#define atomic_subtract_rel_ptr         atomic_subtract_rel_64
#define atomic_store_rel_ptr            atomic_store_rel_64

#include <sys/_atomic_subword.h>

#endif /* _MACHINE_ATOMIC_H_ */
