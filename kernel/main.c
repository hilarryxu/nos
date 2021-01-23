#include <sys/types.h>

#include "nos/dbg.h"
#include "nos/printk.h"

void
kmain(void)
{
  u_char *input = (u_char *)0xB8000;
  u_char color = (0 << 4) | (15 & 0x0F);

  *input++ = 'H'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = 'o'; *input++ = color;
  *input++ = ','; *input++ = color;
  *input++ = ' '; *input++ = color;
  *input++ = 'O'; *input++ = color;
  *input++ = 'S'; *input++ = color;
  *input++ = ' '; *input++ = color;
  *input++ = 'K'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'r'; *input++ = color;
  *input++ = 'n'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = '!'; *input++ = color;

  bochs_putc('Z');

  printk("hijack asdasdasd asda asdas asasdasd asdasd %d", 1234);
}
