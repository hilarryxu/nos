#include "nos/types.h"

void load_kernel(void);

void
loader_main(void)
{
  uchar *input = (uchar *)0xB8000;
  uchar color = (0 << 4) | (15 & 0x0F);

  *input++ = 'H'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = 'o'; *input++ = color;
  *input++ = ','; *input++ = color;
  *input++ = ' '; *input++ = color;
  *input++ = 'L'; *input++ = color;
  *input++ = 'o'; *input++ = color;
  *input++ = 'a'; *input++ = color;
  *input++ = 'd'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'r'; *input++ = color;
  *input++ = '!'; *input++ = color;

  load_kernel();
}

void *
do_memcpy(void *dest, const void *src, int n)
{
  uchar *dest8 = (uchar*)dest;
  uchar *src8 = (uchar*)src;
  int i;

  for (i=0; i<n; i++) {
    dest8[i] = src8[i];
  }
  return dest;
}

void
load_kernel(void)
{
  void (*entry)(void);
  void *kernel_dest = (void*)0x100000;
  void *kernel_src = (void*)0x8200;

  do_memcpy(kernel_dest, kernel_src, 4096);

  entry = (void(*)(void))(kernel_dest);
  entry();
}
