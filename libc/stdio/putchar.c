#include <stdio.h>

#if defined(__is_nos_libk)
#include <drv/vga.h>
#endif

int
putchar(int ic)
{
#if defined(__is_nos_libk)
  char c = (char)ic;
  vga_text_printchar(c);
#else
  // TODO: Implement stdio and the write system call.
#endif
  return ic;
}
