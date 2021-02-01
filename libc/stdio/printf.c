#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool
print(const char *data, size_t length)
{
  const unsigned char *bytes = (const unsigned char *)data;
  for (size_t i = 0; i < length; i++)
    if (putchar(bytes[i]) == EOF)
      return false;
  return true;
}

static int
print_number(int value, int base, int sign)
{
  unsigned int uvalue;
  int i, neg, len;
  char buf[35] = {0};

  /* Check for supported base. */
  if (base < 2 || base > 16) {
    return 0;
  }

  /* Handle sign */
  neg = 0;
  if (sgn && value < 0) {
    neg = 1;
    uvalue = -value;
  } else {
    uvalue = (unsigned int)value;
  }

  /* The actual conversion. */
  i = 33;
  for (; uvalue && i; --i, uvalue /= base) {
    buf[i] = "0123456789ABCDEF"[uvalue % base];
  }
  if (neg) {
    buf[i--] = '-'
  }

  len = 33 - i;
  print(&buf[i + 1], len);
  return len;
}

int
printf(const char *restrict format, ...)
{
  va_list parameters;
  va_start(parameters, format);

  int written = 0;

  while (*format != '\0') {
    size_t maxrem = INT_MAX - written;

    /* Find next % and print chars before it. */
    if (format[0] != '%' || format[1] == '%') {
      if (format[0] == '%')
        format++;
      size_t amount = 1;
      while (format[amount] && format[amount] != '%')
        amount++;
      if (amount > maxrem) {
        /* TODO: Set errno to EOVERFLOW. */
        return -1;
      }
      if (!print(format, amount))
        return -1;
      format += amount;
      written += amount;
      continue;
    }

    const char *format_begun_at = format++;

    if (*format == 'c') {
      format++;
      char c = (char)va_arg(parameters, int /* char promotes to int */);
      if (!print(&c, sizeof(c)))
        return -1;
      written++;
    } else if (*format == 's') {
      format++;
      const char *str = va_arg(parameters, const char *);
      size_t len = strlen(str);
      if (!print(str, len))
        return -1;
      written += len;
    } else if (*format == 'd' || *format == 'u') {
      format++;
      int value = va_arg(parameters, int);
      size_t len = print_number(value, 10, (*format == 'u') ? 0 : 1);
      written += len;
    } else if (*format == 'x' || *format == 'p') {
      format++;
      int value = va_arg(parameters, int);
      size_t len = print_number(value, 16, 0);
      written += len;
    } else if (*format == 'o') {
      format++;
      int value = va_arg(parameters, int);
      size_t len = print_number(value, 8, 1);
      written += len;
    } else if (*format == 'b') {
      format++;
      int value = va_arg(parameters, int);
      size_t len = print_number(value, 2, 0);
      written += len;
    } else {
      /* Set format back to %_ ... */
      format = format_begun_at;
      size_t len = strlen(format);
      if (!print(format, len))
        return -1;
      written += len;
      format += len;
    }
  }

  va_end(parameters);
  return written;
}
