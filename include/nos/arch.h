#ifndef _NOS_ARCH_H
#define _NOS_ARCH_H

#include <stdint.h>
#include <stdbool.h>

#define FL_IF 0x00000200  // Interrupt Flag

#define barrier() asm volatile("" ::: "memory")

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
intr_disable()
{
  asm volatile("cli");
}

static inline void
intr_enable()
{
  asm volatile("sti");
}

static inline void
cpu_hlt()
{
  asm volatile("hlt");
}

static inline void
ltr(uint16_t sel)
{
  asm volatile("ltr %0" ::"r"(sel) : "memory");
}

static inline void
invlpg(void *addr)
{
  asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

static inline bool
__intr_save(void)
{
  if (read_eflags() & FL_IF) {
    intr_disable();
    return 1;
  }
  return 0;
}

static inline void
__intr_restore(bool flag)
{
  if (flag) {
    intr_enable();
  }
}

#define local_intr_save(x)                                                     \
  do {                                                                         \
    x = __intr_save();                                                         \
  } while (0)
#define local_intr_restore(x) __intr_restore(x);

#endif  // !_NOS_ARCH_H
