#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/cdefs.h>
#include <unistd.h>

/* clang-format off */
__BEGIN_DECLS

#define EOF (-1)

#if !defined(__KERNEL__)

int nos_prints(const char *s);
#define cprints nos_prints

static inline _syscall1(int, nos_putc, int, c)
#define cputc nos_putc

#endif /* !__KERNEL__ */

__END_DECLS
/* clang-format on */

#endif /* !_STDIO_H_ */
