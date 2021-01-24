#include "nos/isr.h"

#include "nos/printk.h"

void
isr_handler(struct isr_regs regs)
{
  printk("recieved interrupt: %d\n", regs.int_no);
}
