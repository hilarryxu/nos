#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

#define __nos_libc 1

#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#define __P(protos) protos /* full-blown ANSI C */
#define __CONCAT1(x, y) x##y
#define __CONCAT(x, y) __CONCAT1(x, y)
#define __STRING(x) #x           /* stringify without expanding x */
#define __XSTRING(x) __STRING(x) /* expand x, then stringify */

#endif /* !_SYS_CDEFS_H_ */
