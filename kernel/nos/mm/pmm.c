#include <nos/mm/pmm.h>

#include <nos/debug.h>

static phys_addr_t placement_pos = (phys_addr_t)PLACEMENT_START;

static uint32_t pmm_stack[PMM_MAX_FRAME];
static uint32_t pmm_stack_top = 0;
uint32_t max_frames = 0;

void
pmm_setup()
{
  uint32_t mem_start_addr = 0x100000 * 2;
  uint32_t mem_end_addr = 0x100000 * 4 - 0x2000;
  uint32_t paddr = mem_start_addr;

  while (paddr < mem_end_addr) {
    pmm_free_block(paddr);
    paddr += PMM_FRAME_SIZE;
    max_frames++;
  }
}

phys_addr_t
boot_alloc_block()
{
  ASSERT(placement_pos + PMM_FRAME_SIZE < PLACEMENT_END);
  placement_pos += PMM_FRAME_SIZE;
  return placement_pos - PMM_FRAME_SIZE;
}

phys_addr_t
pmm_alloc_block()
{
  ASSERT(pmm_stack_top > 0);
  uint32_t paddr = pmm_stack[pmm_stack_top--];
  return paddr;
}

void
pmm_free_block(phys_addr_t paddr)
{
  ASSERT(pmm_stack_top < max_frames);
  pmm_stack[++pmm_stack_top] = paddr;
}

uint32_t
get_total_frames()
{
  return max_frames;
}
