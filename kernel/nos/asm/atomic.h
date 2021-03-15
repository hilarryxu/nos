#ifndef _NOS_ASM_ATOMIC_H
#define _NOS_ASM_ATOMIC_H

#include <nos/types.h>
#include <nos/macros.h>

#ifdef CONFIG_SMP
#define LOCK "lock ; "
#else
#define LOCK ""
#endif

typedef struct {
  volatile int counter;
} atomic_t;

#define ATOMIC_INIT(i)                                                         \
  {                                                                            \
    (i)                                                                        \
  }

#define atomic_read(v) ((v)->counter)

#define atomic_set(v, i) (((v)->counter) = (i))

static __always_inline void
atomic_add(int i, atomic_t *v)
{
  asm volatile(LOCK "addl %1,%0" : "=m"(v->counter) : "ir"(i), "m"(v->counter));
}

static __always_inline void
atomic_sub(int i, atomic_t *v)
{
  asm volatile(LOCK "subl %1,%0" : "=m"(v->counter) : "ir"(i), "m"(v->counter));
}

static __always_inline int
atomic_sub_and_test(int i, atomic_t *v)
{
  unsigned char c;

  asm volatile(LOCK "subl %2,%0; sete %1"
               : "=m"(v->counter), "=qm"(c)
               : "ir"(i), "m"(v->counter)
               : "memory");
  return c;
}

static __always_inline void
atomic_inc(atomic_t *v)
{
  asm volatile(LOCK "incl %0" : "=m"(v->counter) : "m"(v->counter));
}

static __always_inline void
atomic_dec(atomic_t *v)
{
  asm volatile(LOCK "decl %0" : "=m"(v->counter) : "m"(v->counter));
}

static __always_inline int
atomic_dec_and_test(atomic_t *v)
{
  unsigned char c;

  asm volatile(LOCK "decl %0; sete %1"
               : "=m"(v->counter), "=qm"(c)
               : "m"(v->counter)
               : "memory");
  return c != 0;
}

static __always_inline int
atomic_inc_and_test(atomic_t *v)
{
  unsigned char c;

  asm volatile(LOCK "incl %0; sete %1"
               : "=m"(v->counter), "=qm"(c)
               : "m"(v->counter)
               : "memory");
  return c != 0;
}

#endif  // !_NOS_ASM_ATOMIC_H
