#ifndef _NOS_SYNC_SYNC_H
#define _NOS_SYNC_SYNC_H

#include <stdbool.h>

#include <nos/arch.h>

static inline bool
__intr_save(void)
{
  if (read_eflags() & FL_IF) {
    intr_disable();
    return 1;
  }
  return 0;
}

static inline void
__intr_restore(bool flag)
{
  if (flag) {
    intr_enable();
  }
}

#define local_intr_save(x)                                                     \
  do {                                                                         \
    x = __intr_save();                                                         \
  } while (0)

#define local_intr_restore(x) __intr_restore(x);

#endif  // !_NOS_SYNC_SYNC_H
