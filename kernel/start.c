#include <stdio.h>

#include <drv/vga.h>
#include <kernel/descriptor_tables.h>

void
kmain(void)
{
  vga_setup();
  gdt_setup();

  printf("Hello %s!", "kernel");
}
