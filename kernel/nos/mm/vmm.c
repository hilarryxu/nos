#include <nos/mm/vmm.h>

#include <stdint.h>

void
vmm_setup()
{
  uint32_t cr0;

  // WOW: 开启分页机制
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= (1 << 31);
  asm volatile("mov %0, %%cr0" : : "r"(cr0));
}
