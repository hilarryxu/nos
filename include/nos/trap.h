#ifndef _NOS_TRAP_H
#define _NOS_TRAP_H

#include <stdint.h>

struct trap_frame {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;

  uint32_t trap_no;
  uint32_t error;

  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t esp;
  uint32_t ss;
};

void handle_interrupt(struct trap_frame *tf);

#endif  // !_NOS_TRAP_H