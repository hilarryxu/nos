#include <stddef.h>
#include <stdarg.h>

#include <nos/drvs/cga.h>

#define MAX_PRINT_INT_BIT_NR 32

static void
kprint_char(char chr)
{
  cga_putchar(chr);
}

static void
kprint_str(char *str)
{
  if (str == NULL)
    str = "(null)";
  while (*str)
    kprint_char(*str++);
}

static void
kprint_uint(unsigned int value, int radix, int uppercase)
{
  const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
  char buf[MAX_PRINT_INT_BIT_NR + 1];
  char *p = buf + MAX_PRINT_INT_BIT_NR;

  *p = '\0';

  do {
    *--p = digits[value % radix];
    value /= radix;
  } while (value);

  kprint_str(p);
}

static void
kprint_int(int value, int radix, int uppercase)
{
  const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
  char buf[MAX_PRINT_INT_BIT_NR + 1];
  char *p = buf + MAX_PRINT_INT_BIT_NR;
  int sign = 0;

  if (value < 0) {
    value = -value;
    sign = 1;
  }
  *p = '\0';

  do {
    *--p = digits[value % radix];
    value /= radix;
  } while (value);

  if (!sign) {
    kprint_str(p);
  } else {
    *--p = '-';
    kprint_str(p);
  }
}

void
vakprintf(char *format, va_list args)
{
  while (*format) {
    if (*format == '%') {
      format++;
      if (!*format)
        return;
      else if (*format == '%')
        kprint_char('%');
      else if (*format == 'b')
        kprint_uint(va_arg(args, unsigned int), 2, 0);
      else if (*format == 'c')
        kprint_char(va_arg(args, unsigned int));
      else if (*format == 'o')
        kprint_uint(va_arg(args, unsigned int), 8, 0);
      else if (*format == 'd')
        kprint_int(va_arg(args, int), 10, 0);
      else if (*format == 'x')
        kprint_uint(va_arg(args, unsigned int), 16, 0);
      else if (*format == 'X')
        kprint_uint(va_arg(args, unsigned int), 16, 1);
      else if (*format == 's')
        kprint_str(va_arg(args, char *));
      else {
        kprint_char('%');
        kprint_char(*format);
      }
    } else
      kprint_char(*format);
    format++;
  }
}

// printk
// 内核编程中使用的 printf 函数（调试必备函数）
void
printk(char *format, ...)
{
  va_list args;

  va_start(args, format);
  vakprintf(format, args);
  va_end(args);
}
