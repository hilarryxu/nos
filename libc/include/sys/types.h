#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>

__BEGIN_DECLS

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

typedef int ssize_t;

typedef int pid_t;

__END_DECLS

#endif /* !_SYS_TYPES_H_ */
