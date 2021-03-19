#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#define EOF (-1)

#if !defined(__KERNEL__)
int nos_prints(const char *s);
#define cprints nos_prints
#endif /* !__KERNEL__ */

__END_DECLS

#endif /* !_STDIO_H_ */
