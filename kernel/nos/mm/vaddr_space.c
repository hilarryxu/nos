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
    if (pgdir[1023] == 0) {
      bzero(pgdir, PAGE_SIZE);
      for (int i = KERNEL_PDE_INDEX; i < PAGES_PER_DIR - 1; i++) {
        pgdir[i] = kernel_pgdir->entries[i];
      }
      pgdir[1023] = (pde_t)V2P(vaddr) | VMM_PRESENT | VMM_WRITABLE;

      if (p_pgdir_phys) {
        *p_pgdir_phys = V2P(vaddr);
      }
      return (struct page_directory *)vaddr;
    }
  }

  return NULL;
}
