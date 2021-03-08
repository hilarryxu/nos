#ifndef _NOS_ARCH_H
#define _NOS_ARCH_H

#include <stdint.h>

static inline void
breakpoint(void)
{
  asm volatile("int3");
}

static inline uint32_t
read_cr2()
{
  uint32_t val;
  asm volatile("movl %%cr2, %0" : "=r"(val));
  return val;
}

static inline uint32_t
read_eflags()
{
  uint32_t eflags;
  asm volatile("pushfl; popl %0" : "=r"(eflags));
  return eflags;
}

static inline void
write_eflags(uint32_t eflags)
{
  asm volatile("pushl %0; popfl" : : "r"(eflags));
}

static inline uint32_t
read_ebp()
{
  uint32_t ebp;
  asm volatile("movl %%ebp, %0" : "=r"(ebp));
  return ebp;
}

static inline uint32_t
read_esp()
{
  uint32_t esp;
  asm volatile("movl %%esp, %0" : "=r"(esp));
  return esp;
}

static inline void
interrupt_disable()
{
  asm volatile("cli");
}

static inline void
interrupt_enable()
{
  asm volatile("sti");
}

static inline void
cpu_hlt()
{
  asm volatile("hlt");
}

#define local_irq_save(x)                                                      \
  asm volatile("pushfl \n\t"                                                   \
               "popl %0 \n\t"                                                  \
               "cli"                                                           \
               : "=g"(x)                                                       \
               :                                                               \
               : "memory")

#define local_irq_restore(x)                                                   \
  asm volatile("pushl %0 \n\t"                                                 \
               "popfl"                                                         \
               :                                                               \
               : "g"(x)                                                        \
               : "memory")

#endif  // !_NOS_ARCH_H
