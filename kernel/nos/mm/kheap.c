#include <nos/mm/kheap.h>

#include <nos/mm/pmm.h>

//---------------------------------------------------------------------
// 初始化内核堆管理
//---------------------------------------------------------------------
int
kheap_setup()
{
  log_debug(LOG_INFO, "[kheap] setup");

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
  UNUSED(size);

  phys_addr_t paddr = pmm_alloc_block();
  return CAST_P2V(paddr);
}

//---------------------------------------------------------------------
// 内核堆释放
//---------------------------------------------------------------------
void
kfree(void *p)
{
  phys_addr_t paddr = CAST_V2P(p);
  pmm_free_block(paddr);
}

//---------------------------------------------------------------------
// 按页对齐分配 npage 个页面
//---------------------------------------------------------------------
void *
kmalloc_ap(int npage)
{
  UNUSED(npage);

  return kmalloc(0);
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
