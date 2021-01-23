#ifndef _STDINT_H_
#define _STDINT_H_

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef int32_t             intptr_t;
typedef uint32_t            uintptr_t;
typedef uint32_t            ptrdiff_t;

#define INT8_MIN          -128
#define INT8_MAX          127
#define UINT8_MAX         255
#define INT16_MIN         -32768
#define INT16_MAX         32767
#define UINT16_MAX        65535
#define INT32_MIN         -2147483648
#define INT32_MAX         2147483647
#define UINT32_MAX        4294967295
#define INT64_MIN         0
#define INT64_MAX         ((uint64_t)-1)
#define UINT64_MAX        ((uint64_t)-1)

#define INTPTR_MIN        -2147483648
#define INTPTR_MAX        2147483647
#define UINTPTR_MAX       -1

#endif  // _STDINT_H_
