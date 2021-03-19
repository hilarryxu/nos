#define __LIBRARY__
#include <unistd.h>
#include <stdarg.h>

int errno;

// int open(const char *pathname, int flags);
// int open(const char *pathname, int flags, mode_t mode);

int
open(const char *pathname, int flags, ...)
{
  register int res;
  va_list arg;

  va_start(arg, flags);
  __asm__("int $0x80"
          : "=a"(res)
          : "0"(__NR_open), "b"(pathname), "c"(flags), "d"(va_arg(arg, int)));
  if (res >= 0)
    return res;
  errno = -res;
  return -1;
}

_syscall1(int, nos_prints, const char *, s)
