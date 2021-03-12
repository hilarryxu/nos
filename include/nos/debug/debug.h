#ifndef _NOS_DEBUG_DEBUG_H
#define _NOS_DEBUG_DEBUG_H

#include <nos/macros.h>

#define MAGIC_BREAK()                                                          \
  do {                                                                         \
    asm("xchgw %bx, %bx\n");                                                   \
  } while (0)

#define LOG_EMERG 0   /* system in unusable */
#define LOG_ALERT 1   /* action must be taken immediately */
#define LOG_CRIT 2    /* critical conditions */
#define LOG_ERR 3     /* error conditions */
#define LOG_WARN 4    /* warning conditions */
#define LOG_NOTICE 5  /* normal but significant condition (default) */
#define LOG_INFO 6    /* informational */
#define LOG_DEBUG 7   /* debug messages */
#define LOG_VERB 8    /* verbose messages */
#define LOG_VVERB 9   /* verbose messages on crack */
#define LOG_VVVERB 10 /* verbose messages on ganga */
#define LOG_PVERB 11  /* periodic verbose messages on crack */

#define LOG_MAX_LEN 256

/*
 * log_stderr   - log to stderr
 * loga         - log always
 * loga_hexdump - log hexdump always
 * log_error    - error log messages
 * log_warn     - warning log messages
 * log_panic    - log messages followed by a panic
 * ...
 * log_debug    - debug log messages based on a log level
 * log_hexdump  - hexadump -C of a log buffer
 */
#ifdef NOS_DEBUG_LOG

#define log_debug(_level, ...)                                                 \
  do {                                                                         \
    if (log_loggable(_level) != 0) {                                           \
      _log(__FILE__, __LINE__, 0, __VA_ARGS__);                                \
    }                                                                          \
  } while (0)

#define log_hexdump(_level, _data, _datalen, ...)                              \
  do {                                                                         \
    if (log_loggable(_level) != 0) {                                           \
      _log(__FILE__, __LINE__, 0, __VA_ARGS__);                                \
      _log_hexdump(__FILE__, __LINE__, (char *)(_data), (int)(_datalen),       \
                   __VA_ARGS__);                                               \
    }                                                                          \
  } while (0)

#else

#define log_debug(_level, ...)
#define log_hexdump(_level, _data, _datalen, ...)

#endif  // NOS_DEBUG_LOG

#define log_stderr(...)                                                        \
  do {                                                                         \
    _log_stderr(__VA_ARGS__);                                                  \
  } while (0)

#define log_safe(...)                                                          \
  do {                                                                         \
    _log_safe(__VA_ARGS__);                                                    \
  } while (0)

#define log_stderr_safe(...)                                                   \
  do {                                                                         \
    _log_stderr_safe(__VA_ARGS__);                                             \
  } while (0)

#define loga(...)                                                              \
  do {                                                                         \
    _log(__FILE__, __LINE__, 0, __VA_ARGS__);                                  \
  } while (0)

#define loga_hexdump(_data, _datalen, ...)                                     \
  do {                                                                         \
    _log(__FILE__, __LINE__, 0, __VA_ARGS__);                                  \
    _log_hexdump(__FILE__, __LINE__, (char *)(_data), (int)(_datalen),         \
                 __VA_ARGS__);                                                 \
  } while (0)

#define log_error(...)                                                         \
  do {                                                                         \
    if (log_loggable(LOG_ALERT) != 0) {                                        \
      _log(__FILE__, __LINE__, 0, __VA_ARGS__);                                \
    }                                                                          \
  } while (0)

#define log_warn(...)                                                          \
  do {                                                                         \
    if (log_loggable(LOG_WARN) != 0) {                                         \
      _log(__FILE__, __LINE__, 0, __VA_ARGS__);                                \
    }                                                                          \
  } while (0)

#define log_panic(...)                                                         \
  do {                                                                         \
    if (log_loggable(LOG_EMERG) != 0) {                                        \
      _log(__FILE__, __LINE__, 1, __VA_ARGS__);                                \
    }                                                                          \
  } while (0)

void debug_setup(int level);

int log_loggable(int level);
void _log(const char *file, int line, int panic, const char *fmt, ...);
void _log_stderr(const char *fmt, ...);
void _log_hexdump(const char *file, int line, char *data, int datalen,
                  const char *fmt, ...);

void stacktrace_print();

#endif  // !_NOS_DEBUG_DEBUG_H
