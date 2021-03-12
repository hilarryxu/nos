#include <stdint.h>

#include <nos/nos.h>

struct stack_frame {
  struct stack_frame *ebp;
  uintptr_t eip;
};

void
stacktrace_print()
{
  struct stack_frame *stack_frame = NULL;
  uintptr_t addr;

  asm volatile("movl %%ebp, %0" : "=r"(stack_frame));

  for (int i = 0; i < 10; i++) {
    if (!stack_frame)
      break;

    addr = stack_frame->eip;
    printk("  0x%X: %s\n", addr, "<not found>");

    stack_frame = stack_frame->ebp;
  }
}
