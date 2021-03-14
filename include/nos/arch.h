#ifndef _NOS_ARCH_H
#define _NOS_ARCH_H

#include <stdint.h>
#include <stdbool.h>

#include <nos/macros.h>

#define FL_IF 0x00000200  // Interrupt Flag

#define barrier() asm volatile("" ::: "memory")

static __always_inline void
breakpoint(void)
{
  asm volatile("int3");
}

static __always_inline uint32_t
read_cr2()
{
  uint32_t val;
  asm volatile("movl %%cr2, %0" : "=r"(val));
  return val;
}

static __always_inline uint32_t
read_eflags()
{
  uint32_t eflags;
  asm volatile("pushfl; popl %0" : "=r"(eflags));
  return eflags;
}

static __always_inline void
write_eflags(uint32_t eflags)
{
  asm volatile("pushl %0; popfl" : : "r"(eflags));
}

static __always_inline uint32_t
read_ebp()
{
  uint32_t ebp;
  asm volatile("movl %%ebp, %0" : "=r"(ebp));
  return ebp;
}

static __always_inline uint32_t
read_esp()
{
  uint32_t esp;
  asm volatile("movl %%esp, %0" : "=r"(esp));
  return esp;
}

static __always_inline void
intr_disable()
{
  asm volatile("cli");
}

static __always_inline void
intr_enable()
{
  asm volatile("sti");
}

static __always_inline void
cpu_hlt()
{
  asm volatile("hlt");
}

static __always_inline void
ltr(uint16_t sel)
{
  asm volatile("ltr %0" ::"r"(sel) : "memory");
}

static __always_inline void
invlpg(uintptr_t vaddr)
{
  asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

#endif  // !_NOS_ARCH_H
