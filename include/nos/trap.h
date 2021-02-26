#ifndef _NOS_TRAP_H
#define _NOS_TRAP_H

#include <stdint.h>

// IRQ 起始编号
#define T_IRQ0 32

// 可编程间隔计时器 Timer
#define IRQ_TIMER 0

// 中断栈帧结构
struct trap_frame {
  // 压入的通用寄存器组
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  // esp 暂时不保存恢复，因为后面多任务示例中
  // 会手工修改 esp 在任务间跳来跳去（非正常用法）
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  // 中断号
  uint32_t trap_no;
  // 错误码
  uint32_t error;

  // 中断触发时 CPU 自己压入的
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  // 有特权级变化时才有如下两项（要切换堆栈）
  uint32_t esp;  // 原来的 ESP
  uint32_t ss;   // 原来的 SS
};

// 派发中断
void handle_interrupt(struct trap_frame *tf);

#endif  // !_NOS_TRAP_H
