#include <strings.h>

#include "nos/descriptor_tables.h"

extern void gdt_flush(uint32_t);

#define NR_GDT_ENTRIES 5

struct gdt_entry gdt_entries[NR_GDT_ENTRIES];
struct gdt_ptr gdt_ptr;

static void
gdt_set_gate(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void
gdt_setup()
{
  gdt_ptr.limit = (sizeof(struct gdt_entry) * NR_GDT_ENTRIES) - 1;
  gdt_ptr.base = (uint32_t)&gdt_entries;

  bzero(&gdt_entries, sizeof(struct gdt_entry) * NR_GDT_ENTRIES);

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

  gdt_flush((uint32_t)&gdt_ptr);
}
