#include <nos/pic.h>

#include <stdint.h>

#include <nos/ioport.h>

// 初始化 PIC
void
pic_setup()
{
  uint8_t offset1 = 0x20;
  uint8_t offset2 = 0x28;
  uint8_t mask1, mask2;

  // 保存当前的中断屏蔽掩码
  mask1 = inb(PIC1_DATA);
  mask2 = inb(PIC1_DATA);

  // ICW1
  // 初始化
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

  // ICW2
  // remap 重映射
  // IRQ0 ~ IRQ15 -> 中断 0x20 ~ 0x2F
  outb(PIC1_DATA, offset1);
  outb(PIC2_DATA, offset2);

  // ICW3
  // 级联方式
  outb(PIC1_DATA, 4);  // 0000_0100b 表示用 IRQ2 和从片相连
  outb(PIC2_DATA, 2);  // 2

  // ICW4
  // 工作模式
  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  // 恢复之前保存的中断屏蔽掩码
  outb(PIC1_DATA, mask1);
  outb(PIC2_DATA, mask2);
}

// 发送 EOI 命令给 PIC
void
pic_send_eoi(int irq)
{
  if (irq >= 8) {
    // IRQ8 ~ IRQ15 发 EOI 给从片
    outb(PIC2_COMMAND, PIC_EOI);
  }
  // 发 EOI 给主片
  outb(PIC1_COMMAND, PIC_EOI);
}
