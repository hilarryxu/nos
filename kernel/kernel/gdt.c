#include <string.h>

#include <kernel/descriptor_tables.h>

extern void gdt_flush(uint32_t);

#define NR_GDT_ENTRIES 64

struct gdt_entry gdt_entries[NR_GDT_ENTRIES];
struct pseudo_desc gdt_desc;

int
gdt_setup()
{
  memset(gdt_entries, 0, NR_GDT_ENTRIES * sizeof(struct gdt_entry));

  // Ring 0
  gdt_set_entry(1, 0x00000000, 0x0FFFFF,
                GDT_SEGMENT | GDT_PRESENT | GDT_CODESEG, PRIV_KERNEL);
  gdt_set_entry(2, 0x00000000, 0x0FFFFF,
                GDT_SEGMENT | GDT_PRESENT | GDT_DATASEG, PRIV_KERNEL);

  // Ring 3
  gdt_set_entry(3, 0x00000000, 0x0FFFFF,
                GDT_SEGMENT | GDT_PRESENT | GDT_CODESEG, PRIV_USER);
  gdt_set_entry(4, 0x00000000, 0x0FFFFF,
                GDT_SEGMENT | GDT_PRESENT | GDT_DATASEG, PRIV_USER);

  gdt_desc.pd_limit = (NR_GDT_ENTRIES * sizeof(struct gdt_entry)) - 1;
  gdt_desc.pd_base = (uint32_t)&gdt_entries;

  gdt_flush((uint32_t)&gdt_desc);

  return 0;
}

void
gdt_set_entry(int segment, uint32_t base, uint32_t limit, uint8_t access,
              enum priv priv)
{
  gdt_entries[segment].base_low = base & 0xFFFF;
  gdt_entries[segment].base_middle = (base >> 16) & 0xFF;
  gdt_entries[segment].base_high = ((base >> 24) & 0xFF);
  // limit 只有 20 位，当下面的 Gr 为 1 时，粒度为一页（4Kib = 2^12）
  // 即当 limit = 0xFFFFF 时表示范围为 0xFFFFFFFF，可寻址 4Gib
  gdt_entries[segment].limit_low = limit & 0xFFFF;
  // limit 的 20-23 位（共 4 位）| (Gr Sz 0 0)
  // 这里设置了 Gr=1 Sz=1，表示 32 位保护模式，且 limit 粒度为一页大小。
  gdt_entries[segment].granularity = ((limit >> 16) & 0x0F) | 0xC0;
  gdt_entries[segment].access = access | ((priv & 3) << 5);
}
