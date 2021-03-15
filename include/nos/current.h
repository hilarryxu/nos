#ifndef _NOS_CURRENT_H
#define _NOS_CURRENT_H

#include <nos/proc/process.h>

static inline struct process *
get_current()
{
  return current_process;
}

#define current get_current()

#endif  // !_NOS_CURRENT_H
