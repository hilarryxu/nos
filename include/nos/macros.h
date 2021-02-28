#ifndef _NOS_MACROS_H
#define _NOS_MACROS_H

#include <stddef.h>  // size_t
#include <stdint.h>

typedef unsigned char u_char;
typedef uint32_t phys_addr_t;

#define NELEMS(a) ((sizeof(a)) / sizeof((a)[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define NOS_ALIGNMENT sizeof(unsigned long)
#define NOS_ALIGN(d, n) (((d) + (n - 1)) & ~(n - 1))
#define NOS_ALIGN_PTR(p, n)                                                    \
  (void *)(((uintptr_t)(p) + ((uintptr_t)n - 1)) & ~((uintptr_t)n - 1))

#define NOS_OK 0
#define NOS_ERROR -1

#define NOS_ASSERT(_x)
#define NOS_NOT_REACHED()

#endif  // !_NOS_MACROS_H
