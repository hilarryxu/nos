#include <string.h>

#include <nos/mm/vmm.h>

//---------------------------------------------------------------------
// 在预留的用户页目录虚拟地址区间给用户进程分配页目录
//---------------------------------------------------------------------
struct page_directory *
vaddr_space_create(phys_addr_t *p_pgdir_phys)
{
  uint32_t vaddr = PGDIR_AREA_BEGIN;
  for (; vaddr < PGDIR_AREA_END; vaddr += PAGE_SIZE) {
    pde_t *pgdir = (pde_t *)vaddr;

    // 寻找首个未使用的页目录虚拟地址
    if (pgdir[1023] == 0) {
      // 清零整页
      bzero(pgdir, PAGE_SIZE);

      // 拷贝内核地址空间
      for (int i = KERNEL_PDE_INDEX; i < PAGES_PER_DIR - 1; i++) {
        pgdir[i] = kernel_pgdir->entries[i];
      }

      // 递归页目录
      pgdir[1023] = (pde_t)V2P(vaddr) | VMM_PRESENT | VMM_WRITABLE | VMM_USER;

      // 返回页目录的物理地址
      if (p_pgdir_phys) {
        *p_pgdir_phys = V2P(vaddr);
      }

      // 返回页目录（内核地址空间下的虚拟地址）
      return (struct page_directory *)vaddr;
    }
  }

  return NULL;
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
  // Note: 指向的页表不用拷贝，这样才能达到应用进程共享内核虚拟空间
  // 拷贝 [3G, 4G) 内核的页目录项
  for (uint32_t i = KERNEL_PDE_INDEX; i < PAGES_PER_DIR; i++) {
    vaddr_space->entries[i] = kernel_pgdir->entries[i];
  }
}
