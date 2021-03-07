#include <stdint.h>
#include <string.h>

#include <nos/nos.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

extern char boot_pgdir[];

struct page_directory *kernel_pgdir;
phys_addr_t kernel_pgdir_phys;

//---------------------------------------------------------------------
// 切换虚拟地址空间
//---------------------------------------------------------------------
void
paging_switch_pgdir(phys_addr_t pgdir)
{
  asm volatile("mov %0, %%cr3" : : "r"(pgdir));
}

//---------------------------------------------------------------------
// 初始化分页机制
//---------------------------------------------------------------------
void
paging_setup()
{
  uint32_t i;
  kernel_pgdir = (struct page_directory *)boot_pgdir;
  kernel_pgdir_phys = CAST_V2P(kernel_pgdir);

  // 去掉低端 [0, 4MB) 的映射
  kernel_pgdir->entries[0] = 0;

  // [3G, 3G + 16MB) -> [0, 16MB) 按 4M 页大小映射
  for (i = KERNEL_PDE_INDEX + 1; i < KERNEL_PDE_INDEX + NR_IDENTITY_MAP; i++) {
    kernel_pgdir->entries[i] = (pde_t)((i - KERNEL_PDE_INDEX) * 0x400000) |
                               VMM_PG_4M | VMM_WRITABLE | VMM_PRESENT;
    loga("i = %d, pde = 0x%X", i, kernel_pgdir->entries[i]);
  }

  // 将内核页目录与 [4MB, 8MB) 区间预留的内核页表数组关联起来
  // 以后就不用再创建内核页表了（浪费了些内存，操作方便一些）
  bzero(CAST_P2V(KERNEL_PG_1), 0x400000);
  for (i = KERNEL_PDE_INDEX + NR_IDENTITY_MAP; i < 1023; i++) {
    phys_addr_t pt_paddr = (phys_addr_t)(KERNEL_PG_1 + (i * PAGE_SIZE));
    kernel_pgdir->entries[i] = (pde_t)pt_paddr | VMM_WRITABLE | VMM_PRESENT;
  }

  // 递归页目录
  kernel_pgdir->entries[1023] =
      (pde_t)kernel_pgdir_phys | VMM_WRITABLE | VMM_PRESENT;
  loga("i = %d, pde = 0x%X", 0, kernel_pgdir->entries[0]);
  loga("i = %d, pde = 0x%X", KERNEL_PDE_INDEX,
       kernel_pgdir->entries[KERNEL_PDE_INDEX]);
  loga("i = %d, pde = 0x%X", 1023, kernel_pgdir->entries[1023]);

  MAGIC_BREAK();

  paging_switch_pgdir(kernel_pgdir_phys);

  // Note: bochs 打印的页映射关系
  // (0) Magic breakpoint
  // Next at t=17566136
  // (0) [0x0010134f] 0008:c010134f (unk. ctxt): leave                     ; c9
  // <bochs:2> info tab
  // cr3: 0x00103000
  // 0xc0000000-0xc03fffff -> 0x00000000-0x003fffff
  // 0xfff00000-0xfff00fff -> 0x00000000-0x00000fff
  // 0xfffff000-0xffffffff -> 0x00103000-0x00103fff
  // MAGIC_BREAK();
}

//---------------------------------------------------------------------
// 拷贝内核地址空间到用户进程的地址空间
//---------------------------------------------------------------------
void
paging_copy_kernel_space(struct page_directory *vaddr_space)
{
  // Note: 指向的页表不用拷贝，这样才能达到应用进程共享内核虚拟空间
  // 拷贝 [3G, 4G) 内核高端区域的页目录项
  for (uint32_t i = KERNEL_PDE_INDEX; i < PAGES_PER_DIR; i++) {
    vaddr_space->entries[i] = kernel_pgdir->entries[i];
  }
}
