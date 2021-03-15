#include <nos/mm/vmm.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <nos/nos.h>
#include <nos/arch.h>
#include <nos/mm/pmm.h>
#include <nos/mm/kheap.h>

// 页目录的虚拟地址（魔法值）
#define MAGIC_PAGE_DIR_ADDR 0xFFFFF000

// 页表的虚拟地址基址（魔法值）
#define MAGIC_PAGE_TABLE_ADDR 0xFFC00000

// 获取页表项的指针
static pte_t *
vmm_get_pte_ptr(uintptr_t vaddr, bool alloc, uint32_t flags)
{
  if (vaddr & 0xFFF)
    log_panic("unaligned vaddr 0x%08x", vaddr);

  uint32_t pde_index = VMM_PDE_INDEX(vaddr);
  uint32_t pte_index = VMM_PTE_INDEX(vaddr);

  struct page_directory *page_dir =
      (struct page_directory *)MAGIC_PAGE_DIR_ADDR;
  struct page_table *page_table =
      (struct page_table *)(MAGIC_PAGE_TABLE_ADDR + (pde_index << 12));

  if (!(page_dir->entries[pde_index] & VMM_PRESENT) && alloc) {
    phys_addr_t page_table_phys = pmm_alloc_block();
    // FIXME: 页表的 flags 是否需要也加到函数参数中
    page_dir->entries[pde_index] = (pde_t)page_table_phys | flags | VMM_PRESENT;
    // 清空页表所占一页内存数据
    bzero((void *)page_table, PAGE_SIZE);
  }

  if (page_dir->entries[pde_index] & VMM_PRESENT) {
    return &(page_table->entries[pte_index]);
  }
  return NULL;
}

//---------------------------------------------------------------------
// 在当前地址空间下建立页映射
//---------------------------------------------------------------------
int
vmm_map_page(uintptr_t vaddr, phys_addr_t paddr, uint32_t flags)
{
  pte_t *pte_ptr = vmm_get_pte_ptr(vaddr, true, flags);
  if (pte_ptr == NULL)
    return -1;

  if (*pte_ptr & VMM_PRESENT) {
    log_panic("failed to map 0x%08x to 0x%08x, already has map 0x%08x "
              "to 0x%08x",
              vaddr, paddr, vaddr, *pte_ptr & PAGE_FRAME_MASK);
  }

  *pte_ptr = (pte_t)paddr | flags | VMM_PRESENT;

  // NOTE: not support 4MB big pages
  invlpg(vaddr);

  return NOS_OK;
}

//---------------------------------------------------------------------
// 在当前地址空间下取消页映射
//---------------------------------------------------------------------
void
vmm_unmap_page(uintptr_t vaddr)
{
  pte_t *pte_ptr = vmm_get_pte_ptr(vaddr, false, 0);

  if (pte_ptr) {
    pmm_free_block(*pte_ptr & PAGE_FRAME_MASK);
    *pte_ptr = 0;
  }
}

#if 0
// 连续建立映射多页
void
vmm_map_pages(uintptr_t vaddr, phys_addr_t paddr, size_t num, uint32_t flags)
{
  for (size_t i = 0; i < num; i++) {
    vmm_map_page(vaddr, paddr, flags);
    vaddr += PAGE_SIZE;
    paddr += PAGE_SIZE;
  }
}

// 连续取消映射多页
void
vmm_unmap_pages(uintptr_t vaddr, size_t num)
{
  for (size_t i = 0; i < num; i++) {
    vmm_unmap_page(vaddr);
    vaddr += PAGE_SIZE;
  }
}
#endif

//---------------------------------------------------------------------
// 在当前地址空间下根据虚拟地址获得其映射对应的物理地址
//---------------------------------------------------------------------
int
vmm_v2p(uintptr_t vaddr, phys_addr_t *p_paddr)
{
  pte_t *pte_ptr = vmm_get_pte_ptr(vaddr, false, 0);

  if (!pte_ptr)
    return -1;

  if (p_paddr) {
    return (((uintptr_t)*pte_ptr) & PAGE_FRAME_MASK) + (vaddr & 0xFFF);
  }

  return NOS_OK;
}

//---------------------------------------------------------------------
// 初始化虚拟内存管理
//---------------------------------------------------------------------
int
vmm_setup()
{
  log_debug(LOG_INFO, "[vmm] setup");

  log_debug(LOG_INFO, "[vmm] done");

  return NOS_OK;
}

//---------------------------------------------------------------------
// 切换地址空间
//---------------------------------------------------------------------
void
vmm_switch_pgdir(phys_addr_t pgdir)
{
  asm volatile("mov %0, %%cr3" : : "r"(pgdir));
}
