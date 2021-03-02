#ifndef _NOS_ARCH_H
#define _NOS_ARCH_H

#include <stdint.h>

static inline void breakpoint() __attribute__((always_inline));
static inline uint32_t read_cr2() __attribute__((always_inline));
static inline uint32_t read_eflags() __attribute__((always_inline));
static inline void write_eflags(uint32_t eflags) __attribute__((always_inline));
static inline uint32_t read_ebp() __attribute__((always_inline));
static inline uint32_t read_esp() __attribute__((always_inline));

#define MAGIC_BREAK asm volatile("xchg %bx, %bx")

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

#endif  // !_NOS_ARCH_H
