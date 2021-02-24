#include <nos/gdt.h>

#include <stdint.h>

// GDT 表项
//
// 大小为 8 字节
// access:
//   3..0 TYPE: 段类型
//   4       S: 描述符类型（0=系统 1=代码或数据）
//   6..5  DPL: 描述符特权级
//   7       P: 段存在
// flags:
//   0 AVL: 系统软件可用位
//   1 保留位
//   2 D/B: 默认大小（0=16位段 1=32位段）
//   3   G: 颗粒度（0=1字节 1=4KB）
struct gdt_entry {
  uint16_t limit_0;  // 段限长 15..0
  uint16_t base_0;   // 基地址 15..0
  uint8_t base_1;    // 基地址 23..16
  uint8_t access;
  uint8_t limit_1 : 4;  // 段限长 19..16
  uint8_t flags : 4;
  uint8_t base_2;  // 基地址 31..24
} __attribute__((packed));

#define GDT_ENTRIES_NR 3
// GDT 表（目前只放了 3 个条目）
static struct gdt_entry gdt[GDT_ENTRIES_NR];

static inline struct gdt_entry *
get_descriptor(uint16_t selector)
{
  uint16_t index = selector >> 3;
  return &gdt[index];
}

// 设置基地址和段限长
static void
set_base_limit(struct gdt_entry *descriptor, uint32_t base, uint32_t limit)
{
  descriptor->base_0 = base & 0xFFFF;
  descriptor->base_1 = (base >> 16) & 0xFF;
  descriptor->base_2 = (base >> 24) & 0xFF;
  descriptor->limit_0 = limit & 0xFFFF;
  descriptor->limit_1 = (limit >> 16) & 0xF;
}

// 设置代码段数据段属性
static void
set_code_data(uint16_t selector, uint32_t base, uint32_t limit,
              uint8_t code_or_data, uint8_t dpl)
{
  struct gdt_entry *descriptor = get_descriptor(selector);
  set_base_limit(descriptor, base, limit);

  // 0xC = 1100b，4KB 颗粒度的 32 位段
  descriptor->flags = 0xC;
  // 0x92 = 1001_0010b
  // 段存在，普通段描述符，代码段可读，数据段可写
  // dpl 描述符特权级
  // code_or_data（1=代码段 0=数据段）
  descriptor->access = 0x92 | ((dpl & 0x3) << 5) | ((code_or_data & 0x1) << 3);
}

// 初始化 GDT
void
gdt_setup()
{
  // 默认第一项为全零
  // 第二项：内核代码段 [0, 0xFFFFF*4KB=4G)
  // 第三项：内核数据段 [0, 0xFFFFFFFF)
  set_code_data(KERNEL_CODE_SELECTOR, 0, 0xFFFFF, 1, DPL_0);
  set_code_data(KERNEL_DATA_SELECTOR, 0, 0xFFFFF, 0, DPL_0);

  struct {
    uint16_t limit;
    void *pointer;
  } __attribute__((packed)) gdtp = {
      // GDT 表总字节数 - 1
      .limit = GDT_ENTRIES_NR * sizeof(struct gdt_entry) - 1,
      // GDT 表指针
      .pointer = gdt,
  };

  // 加载 GDT 描述符
  asm volatile("lgdt %0" : : "m"(gdtp));

  // 更新数据段寄存器 DS,ES,SS
  // 利用 ljmp 长跳转更新代码段寄存器 CS
  asm volatile("mov $0x10, %ax;"
               "mov %ax, %ds;"
               "mov %ax, %es;"
               "mov %ax, %ss;"
               "ljmp $0x8, $.1;"
               ".1:");
}
