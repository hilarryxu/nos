#include <nos/mm/pmm.h>

struct boot_allocator {
  char *start_addr;
  char *free_addr;
};

static struct boot_allocator boot_allocator;

static uint32_t pmm_stack[PMM_MAX_FRAME];
static uint32_t pmm_stack_top = 0;
uint32_t max_frames = 0;

static void
boot_allocator_init(void *start_addr)
{
  start_addr = PTR_ALIGN_UP(start_addr, NOS_ALIGNMENT);
  boot_allocator.start_addr = start_addr;
  boot_allocator.free_addr = start_addr;
}

void *
boot_alloc(size_t size)
{
  void *vaddr = boot_allocator.free_addr;
  boot_allocator.free_addr += ALIGN_UP(size, NOS_ALIGNMENT);
  return vaddr;
}

//---------------------------------------------------------------------
// 初始化物理内存管理
//---------------------------------------------------------------------
void
pmm_setup(phys_addr_t free_addr)
{
  // 暂时没用到
  boot_allocator_init(CAST_P2V(free_addr));

  // [16MB, 32MB)
  uint32_t mem_begin_addr = DEFAULT_PHYS_ADDR_BEGIN;
  uint32_t mem_end_addr = DEFAULT_PHYS_ADDR_END;
  uint32_t paddr = mem_begin_addr;
  while (paddr < mem_end_addr) {
    pmm_free_block(paddr);
    paddr += PMM_FRAME_SIZE;
    max_frames++;
  }
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
  ASSERT(pmm_stack_top <= max_frames);
  pmm_stack[++pmm_stack_top] = paddr;
}

uint32_t
get_total_frames()
{
  return max_frames;
}
