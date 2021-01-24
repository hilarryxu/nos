#include "nos/monitor.h"
#include "nos/descriptor_tables.h"

void
kmain(void)
{
  idt_setup();

  monitor_clear();
  monitor_write("Hello nos kernel!");

  asm volatile("int $0x3");
  asm volatile("int $0x4");
}
