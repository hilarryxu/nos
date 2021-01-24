#ifndef _NOS_DESC_TABLES_H_
#define _NOS_DESC_TABLES_H_

#include <stdint.h>

struct idt_entry {
  uint16_t base_lo;
  uint16_t sel;
  uint8_t always0;
  uint8_t flags;
  uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

#endif /* ifndef _NOS_DESC_TABLES_H_ */
