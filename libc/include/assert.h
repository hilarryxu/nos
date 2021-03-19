#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#if !defined(__KERNEL__)

#ifdef NDEBUG
#define assert(x) (void)0
#else
#include <stdio.h>
#include <stdlib.h>
#define assert(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      printf("Assertion failed: %s, file %s, line %d\n", __STRING(expr),       \
             __FILE__, __LINE__);                                              \
      abort();                                                                 \
    }                                                                          \
  } while (0)
#endif

#endif /* !__KERNEL__ */

__END_DECLS

#endif /* !_ASSERT_H_ */
