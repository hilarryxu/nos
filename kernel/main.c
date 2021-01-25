#include "nos/monitor.h"
#include "nos/descriptor_tables.h"
#include "nos/timer.h"

void
kmain(void)
{
  gdt_setup();
  idt_setup();
  irq_setup();

  monitor_clear();
  monitor_write("Hello nos kernel!");

  asm volatile("int $0x3");
  asm volatile("int $0x4");

  asm volatile("sti");
  timer_setup(50); // 50Hz

  for (;;)
    ;
}
