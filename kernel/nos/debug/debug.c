#include <nos/debug/debug.h>

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

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

void
_log_hexdump(const char *file, int line, char *data, int datalen,
             const char *fmt, ...)
{
  UNUSED(file);
  UNUSED(line);
  UNUSED(fmt);

  char buf[8 * LOG_MAX_LEN];
  int i, off, len, size;

  off = 0;                /* data offset */
  len = 0;                /* length of output buffer */
  size = 8 * LOG_MAX_LEN; /* size of output buffer */

  while (datalen != 0 && (len < size - 1)) {
    char *save, *str;
    unsigned char c;
    int savelen;

    len += snprintf(buf + len, size - len, "%08x  ", off);

    save = data;
    savelen = datalen;

    for (i = 0; datalen != 0 && i < 16; data++, datalen--, i++) {
      c = (unsigned char)(*data);
      str = (i == 7) ? "  " : " ";
      len += snprintf(buf + len, size - len, "%02x%s", c, str);
    }
    for (; i < 16; i++) {
      str = (i == 7) ? "  " : " ";
      len += snprintf(buf + len, size - len, "  %s", str);
    }

    data = save;
    datalen = savelen;

    len += snprintf(buf + len, size - len, "  |");

    for (i = 0; datalen != 0 && i < 16; data++, datalen--, i++) {
      c = (unsigned char)(isprint(*data) ? *data : '.');
      len += snprintf(buf + len, size - len, "%c", c);
    }
    len += snprintf(buf + len, size - len, "|\n");

    off += 16;
  }

  debug_print_str(buf, len);

  if (len >= size - 1) {
    debug_print_str("\n", 1);
  }
}

void
nos_assert(const char *cond, const char *file, int line, int panic)
{
  log_error("assert '%s' failed @ (%s, %d)", cond, file, line);

  if (panic) {
    stacktrace_print();
    abort();
  }
}
