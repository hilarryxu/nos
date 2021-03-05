#include <stdint.h>

#include <nos/nos.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

extern char boot_pgdir[];

struct page_directory *kernel_pgdir;
phys_addr_t kernel_pgdir_phys;

void
paging_switch_pgdir(phys_addr_t pgdir)
{
  asm volatile("mov %0, %%cr3" : : "r"(pgdir));
}

void
paging_setup()
{
  kernel_pgdir = (struct page_directory *)boot_pgdir;
  kernel_pgdir_phys = CAST_V2P(kernel_pgdir);

  // 将内核页目录与 [4MB, 8MB) 区间预留的内核页表数组关联起来
  for (int i = VMM_PDE_INDEX(KERNEL_BASE) + 1; i < 1023; i++) {
    struct page_table *page_table =
        (struct page_table *)(KERNEL_PG_1 + (i * PAGE_SIZE));
    kernel_pgdir->entries[i] = (pde_t)page_table | VMM_WRITABLE | VMM_PRESENT;
  }

  kernel_pgdir->entries[0] = VMM_WRITABLE;
  kernel_pgdir->entries[1] = VMM_WRITABLE;
  kernel_pgdir->entries[1023] =
      (pde_t)kernel_pgdir_phys | VMM_WRITABLE | VMM_PRESENT;

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
  MAGIC_BREAK();
}

void
paging_copy_kernel_space(struct page_directory *vaddr_space)
{
  int i;
  int kpde_start = VMM_PDE_INDEX((uintptr_t)KERNEL_VIRTUAL_START);

  // Note: 指向的页表不用拷贝，这样才能达到应用进程共享内核虚拟空间

  // 拷贝低端对等映射 [0, 8MB) 区域
  for (i = 0; i < NR_IDENTITY_MAP; i++) {
    vaddr_space->entries[i] = kernel_pgdir->entries[i];
  }

  // 拷贝 [3G, 4G) 内核高端区域的页目录项
  for (i = kpde_start; i < PAGES_PER_DIR; i++) {
    vaddr_space->entries[i] = kernel_pgdir->entries[i];
  }
}
