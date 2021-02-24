#include <nos/gdt.h>

#include <stdint.h>

// GDT 表项
struct gdt_entry {
  uint16_t limit_0;
  uint16_t base_0;
  uint8_t base_1;
  uint8_t access;
  uint8_t limit_1 : 4;
  uint8_t flags : 4;
  uint8_t base_2;
} __attribute__((packed));

#define GDT_ENTRIES_NR 3
static struct gdt_entry gdt[GDT_ENTRIES_NR];

static inline struct gdt_entry *
get_descriptor(uint16_t selector)
{
  uint16_t index = selector >> 3;
  return &gdt[index];
}

static void
set_base_limit(struct gdt_entry *descriptor, uint32_t base, uint32_t limit)
{
  descriptor->base_0 = base & 0xFFFF;
  descriptor->base_1 = (base >> 16) & 0xFF;
  descriptor->base_2 = (base >> 24) & 0xFF;
  descriptor->limit_0 = limit & 0xFFFF;
  descriptor->limit_1 = (limit >> 16) & 0xF;
}

static void
set_code_data(uint16_t selector, uint32_t base, uint32_t limit,
              uint8_t code_or_data, uint8_t dpl)
{
  struct gdt_entry *descriptor = get_descriptor(selector);
  set_base_limit(descriptor, base, limit);

  descriptor->flags = 0xC;
  descriptor->access = 0x92 | ((dpl & 0x3) << 5) | ((code_or_data & 0x1) << 3);
}

void
gdt_setup()
{
  set_code_data(KERNEL_CODE_SELECTOR, 0, 0xFFFFF, 1, DPL_0);
  set_code_data(KERNEL_DATA_SELECTOR, 0, 0xFFFFF, 0, DPL_0);

  struct {
    uint16_t limit;
    void *pointer;
  } __attribute__((packed)) gdtp = {
      .limit = GDT_ENTRIES_NR * 8 - 1,
      .pointer = gdt,
  };

  asm volatile("lgdt %0" : : "m"(gdtp));

  asm volatile("mov $0x10, %ax;"
               "mov %ax, %ds;"
               "mov %ax, %es;"
               "mov %ax, %ss;"
               "ljmp $0x8, $.1;"
               ".1:");
}
