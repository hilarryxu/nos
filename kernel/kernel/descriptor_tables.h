#ifndef _KERNEL_DESCRIPTOR_TABLES_H_
#define _KERNEL_DESCRIPTOR_TABLES_H_

#include <stdint.h>

typedef enum { PRIV_KERNEL = 0, PRIV_USER = 3 } priv_e;

struct pseudo_desc {
  uint16_t pd_limit;
  uint32_t pd_base;
} __attribute__((packed));

struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

#endif /* !_KERNEL_DESCRIPTOR_TABLES_H_ */
