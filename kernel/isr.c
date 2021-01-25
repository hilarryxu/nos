#include <strings.h>

#include "nos/isr.h"

#include "nos/monitor.h"
#include "nos/printk.h"
#include "nos/x86.h"

isr_handler_pt interrupt_handlers[256];

void
irq_setup()
{
  bzero(&interrupt_handlers, sizeof(isr_handler_pt) * 256);
}

void
isr_register_handler(uint8_t no, isr_handler_pt handler)
{
  interrupt_handlers[no] = handler;
}

void
isr_handler(struct isr_regs regs)
{
  printk("recieved interrupt: %d\n", regs.int_no);

  if (interrupt_handlers[regs.int_no] != 0) {
    isr_handler_pt handler = interrupt_handlers[regs.int_no];
    handler(regs);
  }
}

void
irq_handler(struct isr_regs regs)
{
  // 32-47
  if (regs.int_no >= 40) {
    // Send EOI to slave.
    outb(0xA0, 0x20);
  }
  // to master
  outb(0x20, 0x20);

  if (interrupt_handlers[regs.int_no] != 0) {
    isr_handler_pt handler = interrupt_handlers[regs.int_no];
    handler(regs);
  }
}
