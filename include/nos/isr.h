#ifndef _NOS_ISR_H_
#define _NOS_ISR_H_

#include <stdint.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

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

typedef void (*isr_handler_pt)(struct isr_regs);
void isr_register_handler(uint8_t no, isr_handler_pt handler);

#endif // _NOS_ISR_H_
