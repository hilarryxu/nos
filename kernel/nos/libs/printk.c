#include <stddef.h>
#include <stdarg.h>

#include <nos/libs/printf.h>
#include <nos/drvs/cga.h>

static void
kprint_str(char *str, int len)
{
  while (len-- && *str)
    cga_putchar(*str++);
}

// printk
// 内核编程中使用的 printf 函数（调试必备函数）
void
printk(char *format, ...)
{
  char buf[1024];
  int len;
  va_list args;

  va_start(args, format);
  len = vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);

  kprint_str(buf, len);
}
