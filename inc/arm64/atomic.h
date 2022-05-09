/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_ARM64_ATOMIC_H
#define __SANDBOX_ARM64_ATOMIC_H

#define ARCH_ATOMIC_OP(op, asm_op, constraint)				\
static inline void							\
arch_atomic_##op(int *counter, int i)					\
{									\
	unsigned long tmp;						\
	int result;							\
									\
	__asm__ volatile(						\
	"	prfm	pstl1strm, %2\n"				\
	"1:	ldxr    %w0, %2\n"					\
	"	"#asm_op"	%w0, %w0, %w3\n"			\
	"	stxr    %w1, %w0, %2\n"					\
	"	cbnz    %w1, 1b\n"					\
	: "=&r" (result), "=&r" (tmp), "+Q" (*counter)			\
	: __stringify(constraint) "r" (i));				\
}

#define ARCH_ATOMIC64_OP(op, asm_op, constraint)			\
static inline void							\
arch_atomic64_##op(int64_t *counter, int64_t i)				\
{									\
	int64_t result;							\
	unsigned long tmp;						\
									\
	__asm__ volatile(						\
	"	prfm    pstl1strm, %2\n"				\
	"1:	ldxr    %0, %2\n"					\
	"	"#asm_op"	%0, %0, %3\n"				\
	"	stxr    %w1, %0, %2\n"					\
	"	cbnz    %w1, 1b"					\
	: "=&r" (result), "=&r" (tmp), "+Q" (*counter)			\
	: __stringify(constraint) "r" (i));				\
}

ARCH_ATOMIC_OP(add,      add, I)
ARCH_ATOMIC_OP(sub,      sub, J)
ARCH_ATOMIC_OP(and,      and, K)
ARCH_ATOMIC_OP(or,        or, K)
ARCH_ATOMIC_OP(xor,      xor, K)
ARCH_ATOMIC_OP(andnot,   bic,  )
ARCH_ATOMIC64_OP(add,    add, I)
ARCH_ATOMIC64_OP(sub,    sub, J)
ARCH_ATOMIC64_OP(and,    and, L)
ARCH_ATOMIC64_OP(or,     orr, L)
ARCH_ATOMIC64_OP(xor,    eor, L)
ARCH_ATOMIC64_OP(andnot, bic,  )

#define ARCH_ATOMIC_OP_RETURN(name, mb, acq, rel, cl, op, asm_op, constraint)	\
static inline int								\
arch_atomic_##op##_return##name(int *counter, int i)				\
{										\
	unsigned long tmp;							\
	int result;								\
										\
	__asm__ volatile(							\
	"	prfm		pstl1strm, %2\n"				\
	"1:	ld" #acq "xr	%w0, %2\n"					\
	"	"#asm_op"	%w0, %w0, %w3\n"				\
	"       st"#rel"xr	%w1, %w0, %2\n"					\
	"	cbnz		%w1, 1b\n"					\
	"	" #mb								\
	: "=&r" (result), "=&r" (tmp), "+Q" (*counter)				\
	: __stringify(constraint) "r" (i)					\
	: cl);									\
										\
	return result;								\
}

#define ARCH_ATOMIC64_OP_RETURN(name, mb, acq, rel, cl, op, asm_op, constraint)	\
static inline long								\
arch_atomic64_##op##_return##name(int64_t *counter, int64_t i)			\
{										\
	int64_t result;								\
	unsigned long tmp;							\
										\
	__asm__ volatile(							\
	"	prfm		pstl1strm, %2\n"				\
	"1:	ld"#acq"xr	%0, %2\n"					\
	"	"#asm_op"	%0, %0, %3\n"					\
	"	st"#rel"xr	%w1, %0, %2\n"					\
	"	cbnz		%w1, 1b\n"					\
	"	" #mb								\
	: "=&r" (result), "=&r" (tmp), "+Q" (*counter)				\
	: __stringify(constraint) "r" (i)					\
	: cl);									\
										\
	return result;								\
}

ARCH_ATOMIC_OP_RETURN(        ,   dmb ish,  , l, "memory", add, add, I)
ARCH_ATOMIC_OP_RETURN(_relaxed,          ,  ,  ,         , add, add, I)
ARCH_ATOMIC_OP_RETURN(_acquire,          , a,  , "memory", add, add, I)
ARCH_ATOMIC_OP_RETURN(_release,          ,  , l, "memory", add, add, I)
ARCH_ATOMIC_OP_RETURN(        ,   dmb ish,  , l, "memory", sub, sub, J)
ARCH_ATOMIC_OP_RETURN(_relaxed,          ,  ,  ,         , sub, sub, J)
ARCH_ATOMIC_OP_RETURN(_acquire,          , a,  , "memory", sub, sub, J)
ARCH_ATOMIC_OP_RETURN(_release,          ,  , l, "memory", sub, sub, J)
ARCH_ATOMIC64_OP_RETURN(        , dmb ish,  , l, "memory", add, add, I)
ARCH_ATOMIC64_OP_RETURN(_relaxed,        ,  ,  ,         , add, add, I)
ARCH_ATOMIC64_OP_RETURN(_acquire,        , a,  , "memory", add, add, I)
ARCH_ATOMIC64_OP_RETURN(_release,        ,  , l, "memory", add, add, I)
ARCH_ATOMIC64_OP_RETURN(        , dmb ish,  , l, "memory", sub, sub, J)
ARCH_ATOMIC64_OP_RETURN(_relaxed,        ,  ,  ,         , sub, sub, J)
ARCH_ATOMIC64_OP_RETURN(_acquire,        , a,  , "memory", sub, sub, J)
ARCH_ATOMIC64_OP_RETURN(_release,        ,  , l, "memory", sub, sub, J)


#define ARCH_ATOMIC_FETCH_OP(name, mb, acq, rel, cl, op, asm_op, constraint)	\
static inline int								\
arch_atomic_fetch_##op##name(int *counter, int i)				\
{										\
	unsigned long tmp;							\
	int val, result;							\
										\
	__asm__ volatile(							\
	"	prfm		pstl1strm, %3\n"				\
	"1:	ld"#acq"xr	%w0, %3\n"					\
	"	"#asm_op"	%w1, %w0, %w4\n"				\
	"	st"#rel"xr	%w2, %w1, %3\n"					\
	"	cbnz		%w2, 1b\n"					\
	"	" #mb								\
	: "=&r" (result), "=&r" (val), "=&r" (tmp), "+Q" (*counter)		\
	: __stringify(constraint) "r" (i)					\
	: cl);									\
										\
	return result;								\
}

#define ARCH_ATOMIC64_FETCH_OP(name, mb, acq, rel, cl, op, asm_op, constraint)	\
static inline long								\
arch_atomic64_fetch_##op##name(int64_t *counter, int64_t i)			\
{                                                                       	\
	int64_t result, val;                                                	\
	unsigned long tmp;                                              	\
										\
	asm volatile(								\
	"	prfm		pstl1strm, %3\n"				\
	"1:	ld"#acq"xr	%0, %3\n"					\
	"	"#asm_op"	%1, %0, %4\n"					\
	"	st"#rel"xr	%w2, %1, %3\n"					\
	"	cbnz		%w2, 1b\n"					\
	"	" #mb								\
	: "=&r" (result), "=&r" (val), "=&r" (tmp), "+Q" (*counter)		\
	: __stringify(constraint) "r" (i)					\
	: cl);									\
										\
	return result;								\
}

ARCH_ATOMIC_FETCH_OP(        ,   dmb ish,  , l, "memory", add,    add, I)
ARCH_ATOMIC_FETCH_OP(_relaxed,          ,  ,  ,         , add,    add, I)
ARCH_ATOMIC_FETCH_OP(_acquire,          , a,  , "memory", add,    add, I)
ARCH_ATOMIC_FETCH_OP(_release,          ,  , l, "memory", add,    add, I)
ARCH_ATOMIC_FETCH_OP(        ,   dmb ish,  , l, "memory", sub,    sub, J)
ARCH_ATOMIC_FETCH_OP(_relaxed,          ,  ,  ,         , sub,    sub, J)
ARCH_ATOMIC_FETCH_OP(_acquire,          , a,  , "memory", sub,    sub, J)
ARCH_ATOMIC_FETCH_OP(_release,          ,  , l, "memory", sub,    sub, J)
ARCH_ATOMIC_FETCH_OP(        ,   dmb ish,  , l, "memory", and,    and, K)
ARCH_ATOMIC_FETCH_OP(_relaxed,          ,  ,  ,         , and,    and, K)
ARCH_ATOMIC_FETCH_OP(_acquire,          , a,  , "memory", and,    and, K)
ARCH_ATOMIC_FETCH_OP(_release,          ,  , l, "memory", and,    and, K)
ARCH_ATOMIC_FETCH_OP(        ,   dmb ish,  , l, "memory", or,     orr, K)
ARCH_ATOMIC_FETCH_OP(_relaxed,          ,  ,  ,         , or,     orr, K)
ARCH_ATOMIC_FETCH_OP(_acquire,          , a,  , "memory", or,     orr, K)
ARCH_ATOMIC_FETCH_OP(_release,          ,  , l, "memory", or,     orr, K)
ARCH_ATOMIC_FETCH_OP(        ,   dmb ish,  , l, "memory", xor,    eor, K)
ARCH_ATOMIC_FETCH_OP(_relaxed,          ,  ,  ,         , xor,    eor, K)
ARCH_ATOMIC_FETCH_OP(_acquire,          , a,  , "memory", xor,    eor, K)
ARCH_ATOMIC_FETCH_OP(_release,          ,  , l, "memory", xor,    eor, K)
ARCH_ATOMIC_FETCH_OP(        ,   dmb ish,  , l, "memory", andnot, bic,  )
ARCH_ATOMIC_FETCH_OP(_relaxed,          ,  ,  ,         , andnot, bic,  )
ARCH_ATOMIC_FETCH_OP(_acquire,          , a,  , "memory", andnot, bic,  )
ARCH_ATOMIC_FETCH_OP(_release,          ,  , l, "memory", andnot, bic,  )
ARCH_ATOMIC64_FETCH_OP(        , dmb ish,  , l, "memory", add,    add, I)
ARCH_ATOMIC64_FETCH_OP(_relaxed,        ,  ,  ,         , add,    add, I)
ARCH_ATOMIC64_FETCH_OP(_acquire,        , a,  , "memory", add,    add, I)
ARCH_ATOMIC64_FETCH_OP(_release,        ,  , l, "memory", add,    add, I)
ARCH_ATOMIC64_FETCH_OP(        , dmb ish,  , l, "memory", sub,    sub, J)
ARCH_ATOMIC64_FETCH_OP(_relaxed,        ,  ,  ,         , sub,    sub, J)
ARCH_ATOMIC64_FETCH_OP(_acquire,        , a,  , "memory", sub,    sub, J)
ARCH_ATOMIC64_FETCH_OP(_release,        ,  , l, "memory", sub,    sub, J)
ARCH_ATOMIC64_FETCH_OP(        , dmb ish,  , l, "memory", and,    and, L)
ARCH_ATOMIC64_FETCH_OP(_relaxed,        ,  ,  ,         , and,    and, L)
ARCH_ATOMIC64_FETCH_OP(_acquire,        , a,  , "memory", and,    and, L)
ARCH_ATOMIC64_FETCH_OP(_release,        ,  , l, "memory", and,    and, L)
ARCH_ATOMIC64_FETCH_OP(        , dmb ish,  , l, "memory", or,     orr, L)
ARCH_ATOMIC64_FETCH_OP(_relaxed,        ,  ,  ,         , or,     orr, L)
ARCH_ATOMIC64_FETCH_OP(_acquire,        , a,  , "memory", or,     orr, L)
ARCH_ATOMIC64_FETCH_OP(_release,        ,  , l, "memory", or,     orr, L)
ARCH_ATOMIC64_FETCH_OP(        , dmb ish,  , l, "memory", xor,    eor, L)
ARCH_ATOMIC64_FETCH_OP(_relaxed,        ,  ,  ,         , xor,    eor, L)
ARCH_ATOMIC64_FETCH_OP(_acquire,        , a,  , "memory", xor,    eor, L)
ARCH_ATOMIC64_FETCH_OP(_release,        ,  , l, "memory", xor,    eor, L)
ARCH_ATOMIC64_FETCH_OP(        , dmb ish,  , l, "memory", andnot, bic,  )
ARCH_ATOMIC64_FETCH_OP(_relaxed,        ,  ,  ,         , andnot, bic,  )
ARCH_ATOMIC64_FETCH_OP(_acquire,        , a,  , "memory", andnot, bic,  )
ARCH_ATOMIC64_FETCH_OP(_release,        ,  , l, "memory", andnot, bic,  )

static inline int64_t
arch_atomic64_dec_if_positive(int64_t *counter)
{
	int64_t result;
	unsigned long tmp;

	__asm__ volatile(
	"	prfm   pstl1strm, %2\n"
	"1:	ldxr   %0, %2\n"
	"	subs   %0, %0, #1\n"
	"	b.lt   2f\n"
	"	stlxr  %w1, %0, %2\n"
	"	cbnz   %w1, 1b\n"
	"	dmb    ish\n"
	"2:"
	: "=&r" (result), "=&r" (tmp), "+Q" (*counter)
	:
	: "cc", "memory");

	return result;
}

#define ARCH_CMPXCHG_OP(w, sfx, name, type, sz, mb, acq, rel, cl, constraint)	\
static inline type							\
arch_cmpxchg_##name##sz(volatile void *ptr,				\
			unsigned long old,				\
			type new)					\
{                                                                       \
	unsigned long tmp;						\
	type oldval;							\
									\
	/*								\
	 * Sub-word sizes require explicit casting so that the compare	\
	 * part of the cmpxchg doesn't end up interpreting non-zero	\
	 * upper bits of the register containing "old".			\
	 */								\
	if (sz < 32)							\
		old = (type)old;					\
                                                                        \
	__asm__ volatile(						\
	"	prfm         pstl1strm, %[v]\n"				\
	"1:	ld"#acq"xr"  #sfx "\t%" #w "[oldval], %[v]\n"		\
	"	eor          %"#w"[tmp], %"#w"[oldval], %"#w"[old]\n"	\
	"	cbnz         %" #w "[tmp], 2f\n"			\
	"	st"#rel"xr"  #sfx "\t%w[tmp], %" #w "[new], %[v]\n"	\
	"	cbnz         %w[tmp], 1b\n"				\
	"	" #mb "\n"						\
	"2:"								\
	: [tmp] "=&r" (tmp), [oldval] "=&r" (oldval),			\
	  [v] "+Q" (*(type *)ptr)					\
	: [old] __stringify(constraint) "r" (old), [new] "r" (new)      \
	: cl);								\
									\
	return oldval;							\
}

ARCH_CMPXCHG_OP(w, b,     , uint8_t,   8,        ,  ,  ,         , K)
ARCH_CMPXCHG_OP(w, h,     , uint16_t, 16,        ,  ,  ,         , K)
ARCH_CMPXCHG_OP(w,  ,     , uint32_t, 32,        ,  ,  ,         , K)
ARCH_CMPXCHG_OP( ,  ,     , uint64_t, 64,        ,  ,  ,         , L)
ARCH_CMPXCHG_OP(w, b, acq_, uint8_t,   8,        , a,  , "memory", K)
ARCH_CMPXCHG_OP(w, h, acq_, uint16_t, 16,        , a,  , "memory", K)
ARCH_CMPXCHG_OP(w,  , acq_, uint32_t, 32,        , a,  , "memory", K)
ARCH_CMPXCHG_OP( ,  , acq_, uint64_t, 64,        , a,  , "memory", L)
ARCH_CMPXCHG_OP(w, b, rel_, uint8_t,   8,        ,  , l, "memory", K)
ARCH_CMPXCHG_OP(w, h, rel_, uint16_t, 16,        ,  , l, "memory", K)
ARCH_CMPXCHG_OP(w,  , rel_, uint32_t, 32,        ,  , l, "memory", K)
ARCH_CMPXCHG_OP( ,  , rel_, uint64_t, 64,        ,  , l, "memory", L)
ARCH_CMPXCHG_OP(w, b,  mb_, uint8_t,   8, dmb ish,  , l, "memory", K)
ARCH_CMPXCHG_OP(w, h,  mb_, uint16_t, 16, dmb ish,  , l, "memory", K)
ARCH_CMPXCHG_OP(w,  ,  mb_, uint32_t, 32, dmb ish,  , l, "memory", K)
ARCH_CMPXCHG_OP( ,  ,  mb_, uint64_t, 64, dmb ish,  , l, "memory", L)

#define ARCH_CMPXCHG_DOUBLE_OP(name, mb, rel, cl)			\
static inline long							\
arch_cmpxchg_double##name(unsigned long old1,				\
			  unsigned long old2,				\
			  unsigned long new1,				\
			  unsigned long new2,				\
			  volatile void *ptr)				\
{									\
	unsigned long tmp, ret;                                         \
                                                                        \
	__asm__ volatile(						\
	"	prfm        pstl1strm, %2\n"				\
	"1:	ldxp        %0, %1, %2\n"				\
	"	eor         %0, %0, %3\n"				\
	"	eor         %1, %1, %4\n"				\
	"	orr         %1, %0, %1\n"				\
	"	cbnz        %1, 2f\n"					\
	"	st"#rel"xp  %w0, %5, %6, %2\n"				\
	"	cbnz    %w0, 1b\n"					\
	"	" #mb "\n"						\
	"2:"								\
	: "=&r" (tmp), "=&r" (ret), "+Q" (*(unsigned long *)ptr)	\
	: "r" (old1), "r" (old2), "r" (new1), "r" (new2)		\
	: cl);								\
									\
	return ret;                                                     \
}

ARCH_CMPXCHG_DOUBLE_OP(   ,        ,  ,         )
ARCH_CMPXCHG_DOUBLE_OP(_mb, dmb ish, l, "memory")

#define arch_atomic_read(v)	READ_ONCE((*v))
#define arch_atomic_set(v, i)	WRITE_ONCE((*v), (i))
#define arch_atomic64_read(v)	READ_ONCE((*v))
#define arch_atomic64_set(v, i)	WRITE_ONCE((*v), (i))

#endif /* __SANDBOX_ARM64_ATOMIC_H */

