#include <nos/mm/pmm.h>

// boot 阶段的简易内存分配器
struct boot_allocator {
  // 起始地址
  char *start_addr;
  // 下次分配可用开始地址
  char *free_addr;
};

// boot 内存分配器
static struct boot_allocator boot_allocator;

// 用栈记录未分配的物理页框
static uint32_t pmm_stack[PMM_MAX_FRAME];

// 栈顶索引值
static uint32_t pmm_stack_top = 0;

// 总页框数
static uint32_t max_frames = 0;

static void
boot_allocator_init(void *start_addr)
{
  start_addr = PTR_ALIGN_UP(start_addr, NOS_ALIGNMENT);
  boot_allocator.start_addr = start_addr;
  boot_allocator.free_addr = start_addr;
}

// 分配内存（不用释放的）
void *
boot_alloc(size_t size)
{
  void *vaddr = boot_allocator.free_addr;
  boot_allocator.free_addr += ALIGN_UP(size, NOS_ALIGNMENT);
  return vaddr;
}

//---------------------------------------------------------------------
// 初始化物理内存管理子系统
//---------------------------------------------------------------------
int
pmm_setup(phys_addr_t free_addr, struct multiboot_info *mb_info)
{
  UNUSED(mb_info);

  log_debug(LOG_INFO, "[pmm] setup");

  // 暂时没用到
  boot_allocator_init(CAST_P2V(free_addr));

  // [16MB, 32MB)
  uint32_t mem_start_addr = DEFAULT_PHYS_ADDR_START;
  uint32_t mem_end_addr = DEFAULT_PHYS_ADDR_END;

  // 将可用内存记录到页框栈中
  uint32_t paddr = mem_start_addr;
  while (paddr < mem_end_addr) {
    pmm_free_block(paddr);
    paddr += PMM_FRAME_SIZE;
    max_frames++;
  }

  log_debug(LOG_INFO, "[pmm] done");

  return NOS_OK;
}

//---------------------------------------------------------------------
// 申请分配一个物理页框
//---------------------------------------------------------------------
phys_addr_t
pmm_alloc_block()
{
  ASSERT(pmm_stack_top > 0);
  // 物理地址出栈
  return pmm_stack[pmm_stack_top--];
}

//---------------------------------------------------------------------
// 释放物理地址对应的页框
//---------------------------------------------------------------------
void
pmm_free_block(phys_addr_t paddr)
{
  ASSERT(pmm_stack_top <= max_frames);
  // 物理地址压栈
  pmm_stack[++pmm_stack_top] = paddr;
}

//---------------------------------------------------------------------
// 获取总的可用物理页框数
//---------------------------------------------------------------------
uint32_t
pmm_get_total_frames()
{
  return max_frames;
}

//---------------------------------------------------------------------
// 获取最大物理内存地址
//---------------------------------------------------------------------
uint64_t
pmm_get_max_phys_addr(struct multiboot_info *mb_info)
{
  UNUSED(mb_info);

  return DEFAULT_PHYS_ADDR_END;
}
