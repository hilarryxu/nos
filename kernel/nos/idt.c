#include <nos/idt.h>

#include <stdint.h>

#include <nos/nos.h>
#include <nos/trap.h>
#include <nos/gdt.h>
#include <nos/pic.h>
#include <nos/ioport.h>
#include <nos/pit.h>
#include <nos/exception.h>
#include <nos/proc/scheduler.h>

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
extern void intr_stub_33(void);

extern void intr_stub_48(void);

extern void sched();

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

// 中断存在标志位（表示有效）
#define IDT_ATTR_PRESENT 0x80

// 中断描述符表总共 256 项
#define IDT_ENTRIES_NR 256
static struct idt_entry idt[IDT_ENTRIES_NR];

// 设置内核才能使用的中断门类型处理函数
static inline void
set_intr_handler(uint8_t idt_no, void *handler)
{
  idt_set_entry(idt_no, KERNEL_CODE_SELECTOR, handler, IDT_INTERRUPT_GATE,
                DPL_0);
}

// 设置中断描述符表项
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

// 初始化 IDT
void
idt_setup()
{
  // Excpetions 异常
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
  // Timer
  set_intr_handler(32, intr_stub_32);
  // 键盘
  set_intr_handler(33, intr_stub_33);

  // syscall
  idt_set_entry(48, KERNEL_CODE_SELECTOR, intr_stub_48, IDT_INTERRUPT_GATE,
                DPL_3);

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
}

struct trap_frame *
syscall(struct trap_frame *tf)
{
  switch (tf->eax) {
  case 0:
    printk("%c", tf->ebx);
    break;
  }

  return tf;
}

// 派发中断
struct trap_frame *
handle_interrupt(struct trap_frame *tf)
{
  struct trap_frame *new_tf = tf;

  if (tf->trap_no <= 0x1F) {
    // 异常 [0, 31]
    printk("Exception %d\n", tf->trap_no);
    if (exception_dispatch(tf) != 0) {
      while (1) {
        asm volatile("cli; hlt");
      }
    }
  } else if (tf->trap_no >= T_IRQ0 && tf->trap_no <= 0x2F) {
    if (tf->trap_no == T_IRQ0 + IRQ_TIMER) {
      g_ticks++;
      // new_tf = schedule(tf);
      // 每次将 esp0 设为新任务的内核栈底
      // 下次中断触发就会在这个新任务的内核栈上保存其状态
      // 然后又切换到另外一个任务的内核堆栈去恢复下一个任务
      // 如此反复
      //
      // 这个 +1 是必须的，不然会栈溢出
      // tss.esp0 = (uint32_t)(new_tf + 1);
      pic_send_eoi(tf->trap_no - T_IRQ0);

      if (current_process && current_process->state == PROCESS_STATE_RUNNING)
        yield();
    } else {
      // IRQs [32, 47]
      pic_send_eoi(tf->trap_no - T_IRQ0);
    }
  } else if (tf->trap_no == 0x30) {
    new_tf = syscall(tf);
  } else {
    // 其他暂未处理的中断号
    printk("Unkown interrupt %d\n", tf->trap_no);
    while (1) {
      asm volatile("cli; hlt");
    }
  }

  return new_tf;
}
