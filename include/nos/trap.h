#ifndef _NOS_TRAP_H
#define _NOS_TRAP_H

#include <stdint.h>

// IRQ 起始编号
#define T_IRQ0 32

// 可编程间隔计时器 Timer
#define IRQ_TIMER 0

// 键盘
#define IRQ_KEYBOARD 1

// 中断栈帧结构
struct trap_frame {
  // 压入的通用寄存器组
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t oesp;  // 被忽略的无用值，popal 不会读取该值覆盖 esp 寄存器
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  // 保存段寄存器（cs, ss 在后面）
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;

  // 中断号
  uint32_t trap_no;
  // 错误码
  uint32_t error_code;

  // 中断触发时 CPU 自己压入的
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  // 有特权级变化时才有如下两项（要切换堆栈）
  uint32_t esp;  // 原来的 ESP
  uint32_t ss;   // 原来的 SS
};

// TSS
struct tss {
  uint32_t prev_tss;
  uint32_t esp0;  // 特权级变化时内核栈的 esp 在这里获取
  uint32_t ss0;   // 一般是保持不变的内核数据段
  uint32_t esp1;
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldtr;
  uint16_t trap;
  uint16_t iomap;
} __attribute__((packed));

// 派发中断
void handle_interrupt(struct trap_frame *tf);

#endif  // _NOS_TRAP_H
