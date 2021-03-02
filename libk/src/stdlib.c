#include <stdio.h>
#include <stdlib.h>

#if defined(_KERNEL_)
#include <nos/nos.h>
#endif

__attribute__((__noreturn__)) void
abort(void)
{
#if defined(_KERNEL_)
  printk("\nkernel: panic: abort()\n");
#endif

  while (1) {
  }
  __builtin_unreachable();
}
