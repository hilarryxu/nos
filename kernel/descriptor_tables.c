#include "nos/descriptor_tables.h"

#define NR_IDT_ENTRY 256

struct idt_entry idt_entries[NR_IDT_ENTRIES];
struct idt_ptr idt_ptr;

static void
idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
  idt_entries[num].base_lo = base & 0xFFFF;
  idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

  idt_entries[num].sel = sel;
  idt_entries[num].always0 = 0;
  // We must uncomment the OR below when we get to using user-mode.
  // It sets the interrupt gate's privilege level to 3.
  idt_entries[num].flags = flags /* | 0x60 */;
}

void
idt_setup()
{
  idt_ptr.limit = (sizeof(struct idt_entry) * NR_IDT_ENTRIES) - 1;
  idt_ptr.base = (uint32_t)&idt_entries;

  bzero(&idt_entries, sizeof(struct idt_entry) * NR_IDT_ENTRIES)
}
