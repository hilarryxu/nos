#include <nos/gdt.h>

#include <stdint.h>

#include <nos/trap.h>

// GDT 表项
//
// 大小为 8 字节
// access:
//   3..0 TYPE: 段类型
//     0 Ac: Accessed bit
//           访问位，CPU访问该段会置 1，我们无需设置
//     1 RW: Readable bit/Writable bit
//           代码段：是否可读
//           数据段：是否可写
//     2 DC: Direction bit/Conforming bit
//           代码段：0 特权级必须完全相等才能执行
//                   1 可以从相等或更低级别特权级执行
//           数据段: 0 向上增长 1 向下增长
//     3 Ex: Executable bit
//           是否可执行（用来区分是代码段还是数据段）
//   4       S: 描述符类型（0=系统描述符 1=代码段或数据段描述符）
//              系统描述符：各种门、TSS等
//   6..5  DPL: 描述符特权级
//   7       P: 段存在
// flags:
//   0 AVL: 系统软件可用位，不用关心
//   1 保留位（64 位系统会用到）
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

#define GDT_ENTRIES_NR 6
// GDT 表
static struct gdt_entry gdt[GDT_ENTRIES_NR];

static struct tss tss = {
    .esp0 = 0,
    .ss0 = KERNEL_DATA_SELECTOR,
};

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

// 安装 TSS 描述符
void
gdt_install_tss(uint32_t base, uint32_t limit)
{
  struct gdt_entry *descriptor = get_descriptor(TSS_SELECTOR);
  set_base_limit(descriptor, base, limit);

  descriptor->flags = 0;
  // 0x89 = 1000_1001b
  // S=0 系统描述符
  // TYPE=1001b TSS 描述符
  descriptor->access = 0x89 | (DPL_0 << 5);
}

// 初始化 GDT
void
gdt_setup()
{
  // 默认第 1 项为全零
  // 第 2 项：内核代码段 [0, 0xFFFFF*4KB=4G)
  // 第 3 项：内核数据段 [0, 0xFFFFFFFF)
  // 第 4 项：用户代码段
  // 第 5 项：用户数据段
  // 第 6 项：TSS
  set_code_data(KERNEL_CODE_SELECTOR, 0, 0xFFFFF, 1, DPL_0);
  set_code_data(KERNEL_DATA_SELECTOR, 0, 0xFFFFF, 0, DPL_0);
  set_code_data(USER_CODE_SELECTOR, 0, 0xFFFFF, 1, DPL_3);
  set_code_data(USER_DATA_SELECTOR, 0, 0xFFFFF, 0, DPL_3);
  uint32_t base = (uint32_t)&tss;
  gdt_install_tss(base, base + sizeof(tss) - 1);

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

  // 因为上面刷新了 GDT 表，所以段选择子需要也刷新成新的值
  // 更新一些用到的数据段寄存器 DS,ES,SS
  // 利用 ljmp 长跳转更新代码段寄存器 CS
  asm volatile("mov $0x10, %ax;"
               "mov %ax, %ds;"
               "mov %ax, %es;"
               "mov %ax, %ss;"
               "ljmp $0x8, $.1;"
               ".1:");
}
