#include <nos/debug.h>

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>

#include <nos/macros.h>
#include <nos/libs/printf.h>
#include <nos/drvs/serial.h>

#define LOG_MAX_LEN 256

static int log_level = LOG_WARN;

static void
debug_print_str(const char *str, int len)
{
  while (len-- && *str)
    serial_write(SERIAL_COM1, *str++);
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
  char buf[4 * LOG_MAX_LEN];
  int len, size;
  va_list args;

  len = 0;
  size = 4 * LOG_MAX_LEN;

  len += snprintf(buf + len, size - len, "%s:%d ", file, line);

  va_start(args, fmt);
  len += vsnprintf(buf + len, size - len, fmt, args);
  va_end(args);

  buf[len++] = '\n';
  debug_print_str(buf, len);

  if (panic) {
    abort();
  }
}

void
_log_stderr(const char *fmt, ...)
{
  char buf[4 * LOG_MAX_LEN];
  int len, size;
  va_list args;

  len = 0;
  size = 4 * LOG_MAX_LEN;

  va_start(args, fmt);
  len += vsnprintf(buf, size, fmt, args);
  va_end(args);

  debug_print_str(buf, len);
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
