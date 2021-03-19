#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#if !defined(__KERNEL__)

extern int errno;

#endif /* !__KERNEL__ */

__END_DECLS

#endif /* !_ERRNO_H_ */
