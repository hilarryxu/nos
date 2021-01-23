#include <stdint.h>

#include "nos/x86.h"

void
bochs_putc(char c)
{
  outb(0xe9, (uint8_t)c);
}
