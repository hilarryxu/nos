#ifndef JOS_INC_X86_H
#define JOS_INC_X86_H

#include <inc/types.h>

static __inline uint8_t inb(int port) __attribute__((always_inline));
static __inline void insl(int port, void *addr, int cnt)
    __attribute__((always_inline));
static __inline void outb(int port, uint8_t data)
    __attribute__((always_inline));
static __inline void outw(int port, uint16_t data)
    __attribute__((always_inline));

static __inline uint8_t
inb(int port)
{
  uint8_t data;
  __asm __volatile("inb %w1,%0" : "=a"(data) : "d"(port));
  return data;
}

static __inline void
insl(int port, void *addr, int cnt)
{
  __asm __volatile("cld\n\trepne\n\tinsl"
                   : "=D"(addr), "=c"(cnt)
                   : "d"(port), "0"(addr), "1"(cnt)
                   : "memory", "cc");
}

static __inline void
outb(int port, uint8_t data)
{
  __asm __volatile("outb %0,%w1" : : "a"(data), "d"(port));
}

static __inline void
outw(int port, uint16_t data)
{
  __asm __volatile("outw %0,%w1" : : "a"(data), "d"(port));
}

#endif /* !JOS_INC_X86_H */
