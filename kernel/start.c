#include <stdio.h>

#include <drv/vga.h>

void
kmain(void)
{
  vga_setup();

  printf("Hello %s!", "kernel");
}
