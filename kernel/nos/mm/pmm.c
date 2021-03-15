#include <nos/mm/pmm.h>

#include <string.h>

// boot 阶段的简易内存分配器
struct boot_allocator {
  // 起始地址
  char *start_addr;
  // 下次分配可用开始地址
  char *free_addr;
};

// 空闲页框头部
struct pmm_block {
  struct pmm_block *next;
};

// 物理内存分配器
struct pmm_allocator {
  // 空闲页框链表
  struct pmm_block *freelist;
};

// boot 内存分配器
static struct boot_allocator boot_allocator;

// 物理内存分配器
static struct pmm_allocator pmm_allocator;

// 总页框数
static unsigned long max_frames = 0;

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
pmm_setup(phys_addr_t page_aligned_free, struct multiboot_info *mb_info)
{
  UNUSED(mb_info);

  log_debug(LOG_INFO, "[pmm] setup");

  // 暂时没用到
  boot_allocator_init(CAST_P2V(page_aligned_free));

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
  struct pmm_block *block;

  block = pmm_allocator.freelist;
  if (block) {
    pmm_allocator.freelist = block->next;
    return CAST_V2P((uintptr_t)block);
  } else {
    return 0;
  }
}

//---------------------------------------------------------------------
// 释放物理地址对应的页框
//---------------------------------------------------------------------
void
pmm_free_block(phys_addr_t paddr)
{
  if (paddr & 0xFFF)
    log_panic("unaligned paddr 0x%08x", paddr);

  uint32_t vaddr = P2V(paddr);
  // memset((void *)vaddr, 1, PAGE_SIZE);
  struct pmm_block *block = (struct pmm_block *)vaddr;
  block->next = pmm_allocator.freelist;
  pmm_allocator.freelist = block;
}

//---------------------------------------------------------------------
// 获取总的可用物理页框数
//---------------------------------------------------------------------
unsigned long
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
