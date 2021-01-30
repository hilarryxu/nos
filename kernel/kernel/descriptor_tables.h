#ifndef _KERNEL_DESCRIPTOR_TABLES_H_
#define _KERNEL_DESCRIPTOR_TABLES_H_

#include <stdint.h>

#define GDT_CODESEG 0x0A
#define GDT_DATASEG 0x02
#define GDT_TSS 0x09
#define GDT_PRESENT 0x80
#define GDT_SEGMENT 0x10

enum priv { PRIV_KERNEL = 0, PRIV_USER = 3 };

struct pseudo_desc {
  uint16_t pd_limit;
  uint32_t pd_base;
} __attribute__((packed));

struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  // flags{4} + limit{16:19}
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

struct selector {
  enum priv priv : 2;
  unsigned ti : 1;
  unsigned index : 13;
} __attribute__((packed));

int gdt_setup();

void gdt_set_entry(int segment, uint32_t base, uint32_t limit, uint8_t access,
                   enum priv priv);

#endif /* !_KERNEL_DESCRIPTOR_TABLES_H_ */
