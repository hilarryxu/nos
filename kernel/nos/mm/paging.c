#include <nos/mm/paging.h>

#include <stdint.h>
#include <string.h>

#include <nos/nos.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

extern char boot_pgdir[];

// 内核页目录虚拟地址 <- boot_pgdir
struct page_directory *kernel_pgdir;

// 内核页目录物理地址
phys_addr_t kernel_pgdir_phys;

//---------------------------------------------------------------------
// 初始化分页机制
//---------------------------------------------------------------------
phys_addr_t
paging_setup(phys_addr_t page_aligned_free, struct multiboot_info *mb_info)
{
  kernel_pgdir = (struct page_directory *)boot_pgdir;
  kernel_pgdir_phys = CAST_V2P(kernel_pgdir);

  // [4MB, 8MB) 的页表区域清零
  bzero((void *)KERNEL_PG_1, KERNEL_PG_1_LIM - KERNEL_PG_1);

  // 去掉低端 [0, 16MB) 的对等映射
  kernel_pgdir->entries[0] = 0;
  kernel_pgdir->entries[1] = 0;
  kernel_pgdir->entries[2] = 0;
  kernel_pgdir->entries[3] = 0;

  // 清零 .bss 段
  bzero((void *)KERNEL_BSS_START, KERNEL_BSS_END - KERNEL_BSS_START);

  // [3G, 3G + mem_end) -> [0, mem_end) 按 4M 页大小映射
  // [3G, 3G + 4MB) boot 阶段已经映射过了，跳过之
  // FIXME(xcc): 改用 KERNEL_PG_1 区域当作内核页表区
  phys_addr_t paddr = SIZE_4MB;
  phys_addr_t max_phys_addr = (phys_addr_t)pmm_get_max_phys_addr(mb_info);
  while (paddr < max_phys_addr) {
    uintptr_t vaddr = P2V(paddr);

    // 分配页表
    struct page_table *pgtbl = (struct page_table *)CAST_P2V(page_aligned_free);
    for (uint32_t i = 0; i < PAGES_PER_TABLE; i++) {
      // 填充页表项
      pgtbl->entries[i] = (pte_t)paddr | VMM_WRITABLE | VMM_PRESENT;
      paddr += PAGE_SIZE;
    }

    // 填充页目录项
    kernel_pgdir->entries[VMM_PDE_INDEX(vaddr)] =
        (pde_t)CAST_V2P(pgtbl) | VMM_WRITABLE | VMM_PRESENT;

    page_aligned_free += PAGE_SIZE;
  }

  // 设置 cr3 寄存器，刷新整个 TLB
  vmm_switch_pgdir(kernel_pgdir_phys);

  return page_aligned_free;
}
