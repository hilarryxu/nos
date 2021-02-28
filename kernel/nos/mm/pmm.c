#include <nos/mm/pmm.h>

static uint32_t pmm_stack[PMM_MAX_PAGE];
static uint32_t pmm_stack_top = 0;

uint32_t physical_page_count = 0;

void
pmm_setup()
{
  uint32_t mem_start_addr = 0x100000 * 4;
  uint32_t mem_end_addr = 0x100000 * 8;
  uint32_t pa = mem_start_addr;

  while (pa < mem_end_addr) {
    pmm_free(pa);
    pa += PAGE_SIZE;
    physical_page_count++;
  }
}

phys_addr_t
pmm_alloc()
{
  uint32_t pa = pmm_stack[pmm_stack_top--];

  return pa;
}

void
pmm_free(phys_addr_t pa)
{
  pmm_stack[++pmm_stack_top] = pa;
}
