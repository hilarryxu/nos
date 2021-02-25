#include <nos/idt.h>

#include <stdint.h>

#include <nos/nos.h>
#include <nos/trap.h>
#include <nos/gdt.h>
#include <nos/ioport.h>

extern void intr_stub_0(void);
extern void intr_stub_1(void);
extern void intr_stub_2(void);
extern void intr_stub_3(void);
extern void intr_stub_4(void);
extern void intr_stub_5(void);
extern void intr_stub_6(void);
extern void intr_stub_7(void);
extern void intr_stub_8(void);
extern void intr_stub_9(void);
extern void intr_stub_10(void);
extern void intr_stub_11(void);
extern void intr_stub_12(void);
extern void intr_stub_13(void);
extern void intr_stub_14(void);
extern void intr_stub_15(void);
extern void intr_stub_16(void);
extern void intr_stub_17(void);
extern void intr_stub_18(void);
extern void intr_stub_19(void);

extern void intr_stub_32(void);

// IDT 表项
//
// 大小为 8 字节
//
// selector:offset 用于找中断服务函数首地址
// selector 要去 GDT 表中查找段基址
//
// type_attr:
//   3..0 TYPE: 门类型
//     32 位模式下常用的有：
//       0101b task gate 任务门
//       1110b interrupt gate 中断门
//       1111b trap gate 陷阱门
//   4       S: Storage Segment 设置为 0
//   6..5  DPL: 特权级（为 0 时只有内核可使用）
//   7       P: 存在位（0 表示该条目无效）
struct idt_entry {
  uint16_t offset_0;  // 段内偏移 15..0
  uint16_t selector;  // 段选择子
  uint8_t zero;       // 未使用字段，全为零
  uint8_t type_attr;  // 属性值
  uint16_t offset_1;  // 段内偏移 31..16
} __attribute__((packed));

#define IDT_ATTR_PRESENT 0x80

#define IDT_ENTRIES_NR 256
static struct idt_entry idt[IDT_ENTRIES_NR];

static inline void
set_intr_handler(uint8_t idt_no, void *handler)
{
  idt_set_entry(idt_no, KERNEL_CODE_SELECTOR, handler, IDT_INTERRUPT_GATE,
                DPL_0);
}

void
idt_set_entry(uint8_t entry_no, uint16_t selector, void *offset, uint8_t type,
              uint8_t dpl)
{
  struct idt_entry *entry = &idt[entry_no];
  entry->selector = selector;
  entry->offset_0 = (uint32_t)offset & 0xFFFF;
  entry->offset_1 = ((uint32_t)offset >> 16) & 0xFFFF;
  entry->type_attr = (type & 0xF) | ((dpl & 0x3) << 5) | IDT_ATTR_PRESENT;
}

void
idt_setup()
{
  // excpetion handlers
  set_intr_handler(0, intr_stub_0);
  set_intr_handler(1, intr_stub_1);
  set_intr_handler(2, intr_stub_2);
  set_intr_handler(3, intr_stub_3);
  set_intr_handler(4, intr_stub_4);
  set_intr_handler(5, intr_stub_5);
  set_intr_handler(6, intr_stub_6);
  set_intr_handler(7, intr_stub_7);
  set_intr_handler(8, intr_stub_8);
  set_intr_handler(9, intr_stub_9);
  set_intr_handler(10, intr_stub_10);
  set_intr_handler(11, intr_stub_11);
  set_intr_handler(12, intr_stub_12);
  set_intr_handler(13, intr_stub_13);
  set_intr_handler(14, intr_stub_14);
  set_intr_handler(15, intr_stub_15);
  set_intr_handler(16, intr_stub_16);
  set_intr_handler(17, intr_stub_17);
  set_intr_handler(18, intr_stub_18);
  set_intr_handler(19, intr_stub_19);

  // IRQ handlers
  set_intr_handler(32, intr_stub_32);

  struct {
    uint16_t limit;
    void *pointer;
  } __attribute__((packed)) idtp = {
      // IDT 表总字节数 - 1
      .limit = IDT_ENTRIES_NR * sizeof(struct idt_entry) - 1,
      // IDT 表指针
      .pointer = idt,
  };

  // 加载 IDT
  asm volatile("lidt %0" : : "m"(idtp));

  // 开中断
  asm volatile("sti");
}

void
handle_interrupt(struct trap_frame *tf)
{
  if (tf->trap_no <= 0x1F) {
    printk("Exception %d\n", tf->trap_no);
    while (1) {
      asm volatile("cli; hlt");
    }
  } else if (tf->trap_no >= 0x20 && tf->trap_no <= 0x2F) {
    if (tf->trap_no >= 0x28) {
      // EOI Slave-PIC
      outb(0xA0, 0x20);
    }
    // EOI Master-PIC
    outb(0x20, 0x20);
  } else {
    printk("Unkown interrupt %d\n", tf->trap_no);
    while (1) {
      asm volatile("cli; hlt");
    }
  }
}