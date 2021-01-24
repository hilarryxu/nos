#include "nos/isr.h"

#include "nos/monitor.h"

void
isr_handler(struct isr_regs regs)
{
  if (regs.int_no == 3) {
    monitor_write("recieved interrupt: int 3\n");
  }
  // printk("recieved interrupt: %d\n", regs.int_no);
}
