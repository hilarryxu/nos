#include <stdint.h>

#include <nos/debug.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

struct page_directory *kernel_pgdir = (struct page_directory *)KERNEL_PGDIR;

void
paging_setup()
{
  int i, j;
  struct page_table *page_table;
  uint32_t cr0;
  uint32_t paddr;

  paddr = 0;
  // 将内核页目录与 4MB 的内核页表数组关联起来
  for (i = 0; i < PAGES_PER_DIR; i++) {
    page_table = (struct page_table *)(KERNEL_PG_1 + (i * PAGE_SIZE));
    kernel_pgdir->entries[i] = (pde_t)page_table | VMM_WRITABLE | VMM_PRESENT;

    // 对等映射 [0, 8MB) 区域
    if (i < NR_IDENTITY_MAP) {
      for (j = 0; j < PAGES_PER_TABLE; j++) {
        page_table->entries[j] = (pte_t)paddr | VMM_WRITABLE | VMM_PRESENT;
        paddr += PAGE_SIZE;
      }
    }
  }

  vmm_activate_pgdir((phys_addr_t)kernel_pgdir);

  // WOW: 开启分页机制
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 1 << 31;
  asm volatile("mov %0, %%cr0" : : "r"(cr0));

  // MAGIC_BREAK;
}
