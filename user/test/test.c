#include <stdint.h>
#include <stdio.h>

int i = 0;

void
_start(void)
{
  int j = i + 5;

  for (; i < j; i++) {
    asm("int $0x30" : : "a"(1000), "b"('0' + i));
  }
  // cprints("hello\n");
  nos_putc('A');

  while (1)
    ;
}
