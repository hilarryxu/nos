#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/cdefs.h>

#define EOF (-1)

__BEGIN_DECLS

int printf(const char *__restrict, ...);
int putchar(int);
int puts(const char *);

__END_DECLS

#endif /* !_STDIO_H_ */
