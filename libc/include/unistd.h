#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <errno.h>

__BEGIN_DECLS

#include <nos/syscall/syscall.h>

#define __syscall_return(type, res)                                            \
  do {                                                                         \
    if ((unsigned long)(res) >= (unsigned long)(-125)) {                       \
      errno = -(res);                                                          \
      res = -1;                                                                \
    }                                                                          \
    return (type)(res);                                                        \
  } while (0)

/* XXX - _foo needs to be __foo, while __NR_bar could be _NR_bar. */
#define _syscall0(type, name)                                                  \
  type name(void)                                                              \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("int $0x80" : "=a"(__res) : "0"(__NR_##name));            \
    __syscall_return(type, __res);                                             \
  }

#define _syscall1(type, name, type1, arg1)                                     \
  type name(type1 arg1)                                                        \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("int $0x80"                                               \
                     : "=a"(__res)                                             \
                     : "0"(__NR_##name), "b"((long)(arg1)));                   \
    __syscall_return(type, __res);                                             \
  }

#define _syscall2(type, name, type1, arg1, type2, arg2)                        \
  type name(type1 arg1, type2 arg2)                                            \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("int $0x80"                                               \
                     : "=a"(__res)                                             \
                     : "0"(__NR_##name), "b"((long)(arg1)),                    \
                       "c"((long)(arg2)));                                     \
    __syscall_return(type, __res);                                             \
  }

#define _syscall3(type, name, type1, arg1, type2, arg2, type3, arg3)           \
  type name(type1 arg1, type2 arg2, type3 arg3)                                \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("int $0x80"                                               \
                     : "=a"(__res)                                             \
                     : "0"(__NR_##name), "b"((long)(arg1)), "c"((long)(arg2)), \
                       "d"((long)(arg3)));                                     \
    __syscall_return(type, __res);                                             \
  }

#define _syscall4(type, name, type1, arg1, type2, arg2, type3, arg3, type4,    \
                  arg4)                                                        \
  type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)                    \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("int $0x80"                                               \
                     : "=a"(__res)                                             \
                     : "0"(__NR_##name), "b"((long)(arg1)), "c"((long)(arg2)), \
                       "d"((long)(arg3)), "S"((long)(arg4)));                  \
    __syscall_return(type, __res);                                             \
  }

#define _syscall5(type, name, type1, arg1, type2, arg2, type3, arg3, type4,    \
                  arg4, type5, arg5)                                           \
  type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5)        \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("int $0x80"                                               \
                     : "=a"(__res)                                             \
                     : "0"(__NR_##name), "b"((long)(arg1)), "c"((long)(arg2)), \
                       "d"((long)(arg3)), "S"((long)(arg4)),                   \
                       "D"((long)(arg5)));                                     \
    __syscall_return(type, __res);                                             \
  }

#define _syscall6(type, name, type1, arg1, type2, arg2, type3, arg3, type4,    \
                  arg4, type5, arg5, type6, arg6)                              \
  type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5,        \
            type6 arg6)                                                        \
  {                                                                            \
    long __res;                                                                \
    __asm__ volatile("push %%ebp ; movl %%eax,%%ebp ; movl %1,%%eax ; int "    \
                     "$0x80 ; pop %%ebp"                                       \
                     : "=a"(__res)                                             \
                     : "i"(__NR_##name), "b"((long)(arg1)), "c"((long)(arg2)), \
                       "d"((long)(arg3)), "S"((long)(arg4)),                   \
                       "D"((long)(arg5)), "0"((long)(arg6)));                  \
    __syscall_return(type, __res);                                             \
  }

/* clang-format off */

/* void exit(int status); */
static inline _syscall1(void, exit, int, status)

/* ssize_t read(int fd, void *buf, size_t count); */
static inline _syscall3(ssize_t, read, int, fd, void *, buf, size_t, count)

/* ssize_t write(int fd, const void *buf, size_t count); */
static inline _syscall3(ssize_t, write, int, fd, const void *, buf, size_t, count)

/* int close(int fd); */
static inline _syscall1(int, close, int, fd)

/* pid_t getpid(); */
static inline _syscall0(pid_t, getpid)

__END_DECLS

/* clang-format on */

#endif /* !_UNISTD_H_ */
