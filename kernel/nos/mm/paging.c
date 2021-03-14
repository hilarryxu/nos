#include <stdint.h>
#include <string.h>

#include <nos/nos.h>
#include <nos/mm/vmm.h>

extern char boot_pgdir[];

struct page_directory *kernel_pgdir;
phys_addr_t kernel_pgdir_phys;

//---------------------------------------------------------------------
// 初始化分页机制
//---------------------------------------------------------------------
void
paging_setup()
{
  uint32_t i;
  kernel_pgdir = (struct page_directory *)boot_pgdir;
  kernel_pgdir_phys = CAST_V2P((uintptr_t)kernel_pgdir);

  // [4MB, 8MB) 的页表区域清零
  bzero((void *)KERNEL_PG_1, KERNEL_PG_1_LIM - KERNEL_PG_1);

  // 去掉低端 [0, 16MB) 的对等映射
  kernel_pgdir->entries[0] = 0;
  kernel_pgdir->entries[1] = 0;
  kernel_pgdir->entries[2] = 0;
  kernel_pgdir->entries[3] = 0;

  // FIXME: 清零 .bss 段
  // bzero((void *)KERNEL_BSS_START, KERNEL_BSS_END - KERNEL_BSS_START);

  // [3G, 3G + 16MB) -> [0, 16MB) 按 4M 页大小映射
  // [3G, 3G + 4MB) boot 阶段已经映射过了，跳过之
  for (i = KERNEL_PDE_INDEX + 1; i < KERNEL_PDE_INDEX + NR_IDENTITY_MAP; i++) {
    kernel_pgdir->entries[i] = (pde_t)((i - KERNEL_PDE_INDEX) * SIZE_4MB) |
                               VMM_PG_4M | VMM_WRITABLE | VMM_PRESENT;
    loga("pde[%d] = 0x%08x", i, kernel_pgdir->entries[i]);
  }

  // 将内核页目录与 [4MB, 8MB) 区间预留的内核页表数组关联起来
  // 以后就不用再创建内核页表了（浪费了些内存，操作方便一些）
  bzero(CAST_P2V(KERNEL_PG_1), SIZE_4MB);
  for (i = KERNEL_PDE_INDEX + NR_IDENTITY_MAP; i < 1023; i++) {
    phys_addr_t pt_paddr = (phys_addr_t)(KERNEL_PG_1 + (i * PAGE_SIZE));
    kernel_pgdir->entries[i] = (pde_t)pt_paddr | VMM_WRITABLE | VMM_PRESENT;
  }

  // 递归页目录
  kernel_pgdir->entries[1023] =
      (pde_t)kernel_pgdir_phys | VMM_WRITABLE | VMM_PRESENT;

  loga("pde[%d] = 0x%08x", 0, kernel_pgdir->entries[0]);
  loga("pde[%d] = 0x%08x", KERNEL_PDE_INDEX,
       kernel_pgdir->entries[KERNEL_PDE_INDEX]);
  loga("pde[%d] = 0x%08x", 1023, kernel_pgdir->entries[1023]);

  // 设置 cr3 寄存器，刷新整个 TLB
  vmm_switch_pgdir(kernel_pgdir_phys);
}
