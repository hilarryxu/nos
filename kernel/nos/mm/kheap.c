#include <nos/mm/kheap.h>

#include <nos/mm/pmm.h>

#define SIZE_ALIGNMENT NOS_ALIGNMENT

struct kheap_allocator {
  char *alloc_ptr;
  size_t remaining_bytes;
};

static struct kheap_allocator kheap_allocator;

//---------------------------------------------------------------------
// 初始化内核堆管理
//---------------------------------------------------------------------
int
kheap_setup()
{
  log_debug(LOG_INFO, "[kheap] setup");

  phys_addr_t paddr = pmm_alloc_block();
  kheap_allocator.alloc_ptr = CAST_P2V(paddr);
  kheap_allocator.remaining_bytes = PAGE_SIZE;

  log_debug(LOG_INFO, "[kheap] done");

  return NOS_OK;
}

//---------------------------------------------------------------------
// 内核堆分配
//---------------------------------------------------------------------
void *
kmalloc(size_t size)
{
  ASSERT(size <= PAGE_SIZE);

  if (size == 0)
    return NULL;

  int align = SIZE_ALIGNMENT;
  size_t current_mod = (uintptr_t)(kheap_allocator.alloc_ptr) & (align - 1);
  size_t slop = (current_mod == 0 ? 0 : align - current_mod);
  size_t needed = size + slop;
  char *result;

  if (needed <= kheap_allocator.remaining_bytes) {
    result = kheap_allocator.alloc_ptr + slop;
    kheap_allocator.alloc_ptr += needed;
    kheap_allocator.remaining_bytes -= needed;
  } else {
    phys_addr_t paddr = pmm_alloc_block();
    kheap_allocator.alloc_ptr = CAST_P2V(paddr);
    kheap_allocator.remaining_bytes = PAGE_SIZE;

    return kmalloc(size);
  }

  ASSERT(((uintptr_t)result & (align - 1)) == 0);
  return result;
}

//---------------------------------------------------------------------
// 内核堆释放
//---------------------------------------------------------------------
void
kfree(void *p)
{
  UNUSED(p);
}

//---------------------------------------------------------------------
// 按页对齐分配 npage 个页面
//---------------------------------------------------------------------
void *
kmalloc_ap(int npage)
{
  UNUSED(npage);

  return kmalloc(npage * PAGE_SIZE);
}

//---------------------------------------------------------------------
// 按页对齐释放
//---------------------------------------------------------------------
void
kfree_ap(void *p, int npage)
{
  UNUSED(npage);

  kfree(p);
}
