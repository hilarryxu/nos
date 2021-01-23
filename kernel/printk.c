#include <stdarg.h>

#include "nos/vsnprintf.h"
#include "nos/dbg.h"

#define BUF_SIZE 512

int snprintf(char *str, size_t size, const char *fmt, ...)
{
  va_list args;
  int ret;
  va_start(args, fmt);
  ret = vsnprintf(str, size, fmt, args);
  va_end(args);

  return ret;
}

int printk(char *fmt, ...)
{
  char buf[BUF_SIZE];
  va_list args;
  int i, n;

  va_start(args, fmt);
  n = vsnprintf(buf, BUF_SIZE, fmt, args);
  va_end(args);

  for (i = 0; i < n; i++) {
    bochs_putc(buf[i]);
  }
  return n;
}
