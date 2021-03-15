#ifndef _NOS_ASM_BITOPS_H
#define _NOS_ASM_BITOPS_H

#include <nos/types.h>

#define BITS_TO_LONGS(bits) (((bits) + BITS_PER_LONG - 1) / BITS_PER_LONG)

#define DECLARE_BITMAP(name, bits) unsigned long name[BITS_TO_LONGS(bits)]

#define CLEAR_BITMAP(name, bits)                                               \
  memset(name, 0, BITS_TO_LONGS(bits) * sizeof(unsigned long))
#endif

#ifdef CONFIG_SMP
#define LOCK_PREFIX "lock ; "
#else
#define LOCK_PREFIX ""
#endif

#define ADDR (*(volatile long *)addr)

static __always_inline void
set_bit(int nr, volatile unsigned long *addr)
{
  asm volatile(LOCK_PREFIX "btsl %1,%0" : "=m"(ADDR) : "Ir"(nr));
}

static __always_inline void
clear_bit(int nr, volatile unsigned long *addr)
{
  asm volatile(LOCK_PREFIX "btrl %1,%0" : "=m"(ADDR) : "Ir"(nr));
}

static __always_inline void
change_bit(int nr, volatile unsigned long *addr)
{
  asm volatile(LOCK_PREFIX "btcl %1,%0" : "=m"(ADDR) : "Ir"(nr));
}

static __always_inline int
test_bit(int nr, const volatile unsigned long *addr)
{
  int oldbit;

  asm volatile("btl %2,%1\n\tsbbl %0,%0" : "=r"(oldbit) : "m"(ADDR), "Ir"(nr));
  return oldbit;
}

static __always_inline int
test_and_set_bit(int nr, volatile unsigned long *addr)
{
  int oldbit;

  asm volatile(LOCK_PREFIX "btsl %2,%1\n\tsbbl %0,%0"
               : "=r"(oldbit), "=m"(ADDR)
               : "Ir"(nr)
               : "memory");
  return oldbit;
}

static __always_inline int
test_and_clear_bit(int nr, volatile unsigned long *addr)
{
  int oldbit;

  asm volatile(LOCK_PREFIX "btrl %2,%1\n\tsbbl %0,%0"
               : "=r"(oldbit), "=m"(ADDR)
               : "Ir"(nr)
               : "memory");
  return oldbit;
}

static __always_inline int
test_and_change_bit(int nr, volatile unsigned long *addr)
{
  int oldbit;

  asm volatile(LOCK_PREFIX "btcl %2,%1\n\tsbbl %0,%0"
               : "=r"(oldbit), "=m"(ADDR)
               : "Ir"(nr)
               : "memory");
  return oldbit;
}

#undef ADDR

static __always_inline int
find_first_zero_bit(unsigned long *addr, unsigned size)
{
  int d0, d1, d2;
  int res;

  if (!size)
    return 0;
  /* This looks at memory. Mark it volatile to tell gcc not to move it around */
  asm volatile("movl $-1,%%eax\n\t"
               "xorl %%edx,%%edx\n\t"
               "repe; scasl\n\t"
               "je 1f\n\t"
               "xorl -4(%%edi),%%eax\n\t"
               "subl $4,%%edi\n\t"
               "bsfl %%eax,%%edx\n"
               "1:\tsubl %%ebx,%%edi\n\t"
               "shll $3,%%edi\n\t"
               "addl %%edi,%%edx"
               : "=d"(res), "=&c"(d0), "=&D"(d1), "=&a"(d2)
               : "1"((size + 31) >> 5), "2"(addr), "b"(addr));
  return res;
}

#endif  // !_NOS_ASM_BITOPS_H
