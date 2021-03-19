#ifndef _FCNTL_H_
#define _FCNTL_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#if !defined(__KERNEL__)

#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02

int open(const char *pathname, int flags, ...);

#endif /* !__KERNEL__ */

__END_DECLS

#endif /* !_FCNTL_H_ */
