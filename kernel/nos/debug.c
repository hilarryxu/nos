#include <nos/debug.h>

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>

#include <nos/macros.h>
#include <nos/drvs/serial.h>

#define MAX_PRINT_INT_BIT_NR 32

static int log_level = LOG_WARN;

static void
kprint_char(char chr)
{
  serial_write(SERIAL_COM1, chr);
}

static void
kprint_str(const char *str)
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

static void
log_vakprintf(const char *format, va_list args)
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
    } else {
      kprint_char(*format);
    }
    format++;
  }
}

void
log_printk(const char *format, ...)
{
  va_list args;

  va_start(args, format);
  log_vakprintf(format, args);
  va_end(args);
}

void
debug_setup(int level)
{
  log_level = MAX(LOG_EMERG, MIN(level, LOG_PVERB));
}

int
log_loggable(int level)
{
  if (level > log_level) {
    return 0;
  }

  return 1;
}

void
_log(const char *file, int line, int panic, const char *fmt, ...)
{
  va_list args;

  kprint_str(file);
  kprint_char(':');
  kprint_uint(line, 10, 0);
  kprint_char(' ');

  va_start(args, fmt);
  log_vakprintf(fmt, args);
  va_end(args);

  if (panic) {
    abort();
  }
}

void
_log_stderr(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  log_vakprintf(fmt, args);
  va_end(args);
}

#if 0
void
_log_hexdump(const char *file, int line, char *data, int datalen,
             const char *fmt, ...)
{
  // TODO(xcc): code here
}
#endif

void
nos_assert(const char *cond, const char *file, int line, int panic)
{
  log_error("assert '%s' failed @ (%s, %d)", cond, file, line);

  if (panic) {
    // nos_stacktrace(1);
    abort();
  }
}
