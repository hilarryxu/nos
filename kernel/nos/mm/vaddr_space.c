#include <string.h>

#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

//---------------------------------------------------------------------
// 在预留的用户页目录虚拟地址区间给用户进程分配页目录
//---------------------------------------------------------------------
struct page_directory *
vaddr_space_create(phys_addr_t *p_pgdir_phys)
{
  phys_addr_t paddr = pmm_alloc_block();
  if (!paddr)
    return NULL;

  pde_t *pgdir = (pde_t *)P2V(paddr);

  // 清零整页
  bzero(pgdir, PAGE_SIZE);

  // 拷贝内核地址空间
  for (int i = KERNEL_PDE_INDEX; i < PAGES_PER_DIR; i++) {
    pgdir[i] = kernel_pgdir->entries[i];
  }

  // 返回页目录的物理地址
  if (p_pgdir_phys) {
    *p_pgdir_phys = paddr;
  }

  // 返回页目录（内核地址空间下的虚拟地址）
  return (struct page_directory *)pgdir;
}

// TODO: 待实现
int
vaddr_space_destroy(struct page_directory *vaddr_space)
{
  UNUSED(vaddr_space);

  return NOS_OK;
}

//---------------------------------------------------------------------
// 拷贝内核地址空间到用户进程的地址空间
//---------------------------------------------------------------------
void
copy_kernel_space(struct page_directory *vaddr_space)
{
  // NOTE: 指向的页表不用拷贝，这样才能达到应用进程共享内核虚拟空间
  // 拷贝 [3G, 4G) 内核的页目录项
  for (uint32_t i = KERNEL_PDE_INDEX; i < PAGES_PER_DIR; i++) {
    vaddr_space->entries[i] = kernel_pgdir->entries[i];
  }
}
