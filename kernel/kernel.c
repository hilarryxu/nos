#include "nos/types.h"

void
kmain(void)
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
}
