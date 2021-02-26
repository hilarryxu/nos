#ifndef _NOS_IDT_H
#define _NOS_IDT_H

#include <stdint.h>

// 门类型
enum idt_gate_type {
  // 任务门
  IDT_TASK_GATE = 0x5,
  // 中断门
  IDT_INTERRUPT_GATE = 0xE,
  // 陷阱门
  IDT_TRAP_GATE = 0xF,
};

// 初始化 IDT
void idt_setup();

// 设置 IDT 表项
void idt_set_entry(uint8_t entry_no, uint16_t selector, void *offset,
                   uint8_t type, uint8_t dpl);

#endif  // !_NOS_IDT_H
