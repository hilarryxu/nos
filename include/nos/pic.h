#ifndef _NOS_PIC_H
#define _NOS_PIC_H

// 主 PIC 的 IO 端口基址
#define PIC1 0x20
// 从 PIC 的 IO 端口基址
#define PIC2 0xA0

// 命令、数据 IO 端口地址
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

// IRQ 中断处理完后需要发送的命令码
#define PIC_EOI 0x20  // End-of-interrupt command code

#define ICW1_INIT 0x10  // 初始化命令
#define ICW1_ICW4 0x01  // 需要 ICW4

#define ICW4_8086 0x01  // 8086/88 (MCS-80/85) mode

// 初始化 PIC
void pic_setup();

// 发送 EOI 命令给 PIC
void pic_send_eoi(int irq);

#endif  // !_NOS_PIC_H
