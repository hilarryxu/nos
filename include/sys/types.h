#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#define NULL ((void*)0)

typedef unsigned int       size_t;
typedef signed int         ssize_t;
typedef int                off_t;
typedef long long          off64_t;

#define __USE_BSD

#ifdef __USE_BSD
  #ifndef __u_char_defined
    #define __u_char_defined
    typedef unsigned char u_char;
    typedef unsigned short u_short;
    typedef unsigned int u_int;
    typedef unsigned long u_long;
  #endif
#endif

#endif  // _SYS_TYPES_H_
