#ifndef _NOS_IDT_H
#define _NOS_IDT_H

#include <stdint.h>

enum idt_gate_type {
  IDT_TASK_GATE = 0x5,
  IDT_INTERRUPT_GATE = 0xE,
  IDT_TRAP_GATE = 0xF,
};

// 初始化 IDT
void idt_setup();
// 设置 IDT 表项
void idt_set_entry(uint8_t entry_no, uint16_t selector, void *offset,
                   uint8_t type, uint8_t dpl);

#endif  // !_NOS_IDT_H
