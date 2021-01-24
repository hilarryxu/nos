#ifndef _NOS_ISR_H_
#define _NOS_ISR_H_

#include <stdint.h>

struct isr_regs {
  // Data segment selector
  uint32_t ds;
  // Pushed by pusha.
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  // Interrupt number and error code (if applicable)
  uint32_t int_no, err_code;
  // Pushed by the processor automatically.
  uint32_t eip, cs, eflags, useresp, ss;
};

#endif // _NOS_ISR_H_
