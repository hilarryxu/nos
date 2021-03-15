#ifndef _NOS_TYPES_H
#define _NOS_TYPES_H

#define _BSD_SOURCE

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned u_int32_t;
typedef char *caddr_t;
typedef unsigned char u_char;
typedef unsigned short u_short, ushort;
typedef unsigned u_int, uint;
typedef unsigned long u_long, ulong;
typedef long long quad_t;
typedef unsigned long long u_quad_t;
#endif  // _BSD_SOURCE

#define BITS_PER_LONG 32

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif  // !_SIZE_T

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int ssize_t;
#endif  // !_SSIZE_T

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t;
#endif  // !_PTRDIFF_T

typedef u_int32_t phys_addr_t;
typedef int pid_t;
typedef long off_t;

#endif  // !_NOS_TYPES_H
