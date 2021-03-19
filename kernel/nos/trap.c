#include <nos/trap.h>

#include <stdbool.h>

#include <nos/nos.h>
#include <nos/current.h>
#include <nos/pic.h>
#include <nos/exception.h>
#include <nos/gdt.h>
#include <nos/pit.h>
#include <nos/sched/sched.h>
#include <nos/syscall/syscall.h>
#include <nos/drvs/keyboard.h>

static inline bool
is_trap_in_kernel(struct trap_frame *tf)
{
  return tf->cs == KERNEL_CODE_SELECTOR;
}

static void
trap_dispatch(struct trap_frame *tf)
{
  if (tf->trap_no <= 0x1F) {
    // 异常 [0, 31]
    printk("Exception %d\n", tf->trap_no);
    if (exception_dispatch(tf) != 0) {
      while (1) {
        asm volatile("cli; hlt");
      }
    }
  } else if (tf->trap_no >= T_IRQ0 && tf->trap_no <= 0x2F) {
    pic_send_eoi(tf->trap_no - T_IRQ0);

    if (tf->trap_no == T_IRQ0 + IRQ_TIMER) {
      g_ticks++;
      // 时钟来了就准备进程调度
      // 后续可以计算时间片来设置是否需要调度
      if (g_ticks % 100 == 0) {
        current->need_resched = true;
      }
    } else if (tf->trap_no == T_IRQ0 + IRQ_KEYBOARD) {
      keyboard_interrupt(tf);
    }
  } else if (tf->trap_no == T_SYSCALL) {
    syscall_dispatch(tf);
  } else {
    // 其他暂未处理的中断号
    printk("Unkown interrupt %d\n", tf->trap_no);
    while (1) {
      asm volatile("cli; hlt");
    }
  }
}

//---------------------------------------------------------------------
// 派发中断
//---------------------------------------------------------------------
void
handle_interrupt(struct trap_frame *tf)
{
  if (current == NULL) {
    trap_dispatch(tf);
  } else {
    bool in_kernel = is_trap_in_kernel(tf);
    trap_dispatch(tf);
    if (!in_kernel) {
      // FIXME: check current exit flag
      if (current->need_resched)
        schedule();
    }
  }
}
