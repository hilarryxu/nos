#ifndef _NOS_MACROS_H
#define _NOS_MACROS_H

#include <stddef.h>  // size_t
#include <stdint.h>

typedef unsigned char u_char;
typedef uint32_t phys_addr_t;

#define NELEMS(a) ((sizeof(a)) / sizeof((a)[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// {4095, 4096, 4097} = {0, 4096, 4096}
#define ALIGN_DOWN(base, size) ((base) & -((__typeof__(base))(size)))
// {4095, 4096, 4097} = {4096, 4096, 8192}
// Note: 这里 size 会被展开两次，可能有副作用
#define ALIGN_UP(base, size) ALIGN_DOWN((base) + (size)-1, (size))

#define PTR_ALIGN_DOWN(base, size)                                             \
  ((__typeof__(base))ALIGN_DOWN((uintptr_t)(base), (size)))
#define PTR_ALIGN_UP(base, size)                                               \
  ((__typeof__(base))ALIGN_UP((uintptr_t)(base), (size)))

#define NOS_ALIGNMENT sizeof(unsigned long)
#define NOS_ALIGN(d, n) (((d) + (n - 1)) & ~(n - 1))
#define NOS_ALIGN_PTR(p, n)                                                    \
  (void *)(((uintptr_t)(p) + ((uintptr_t)n - 1)) & ~((uintptr_t)n - 1))

#define NOS_OK 0
#define NOS_ERROR -1

#ifdef NOS_ASSERT_PANIC

#define ASSERT(_x)                                                             \
  do {                                                                         \
    if (!(_x)) {                                                               \
      nos_assert(#_x, __FILE__, __LINE__, 1);                                  \
    }                                                                          \
  } while (0)

#define NOT_REACHED() ASSERT(0)

#elif NOS_ASSERT_LOG

#define ASSERT(_x)                                                             \
  do {                                                                         \
    if (!(_x)) {                                                               \
      nos_assert(#_x, __FILE__, __LINE__, 0);                                  \
    }                                                                          \
  } while (0)

#define NOT_REACHED() ASSERT(0)

#else

#define ASSERT(_x)

#define NOT_REACHED()

#endif  // NOS_ASSERT_PANIC

void nos_assert(const char *cond, const char *file, int line, int panic);

#endif  // !_NOS_MACROS_H
