#ifndef _NOS_MM_VMM_H
#define _NOS_MM_VMM_H

#include <nos/nos.h>
#include <nos/mm/memlayout.h>

#define PAGES_PER_DIR 1024
#define PAGES_PER_TABLE 1024

// 虚拟地址 -> pde_index 页目录项索引值
#define VMM_PDE_INDEX(vaddr) (((uint32_t)(vaddr) >> 22) & 0x3FF)

// 虚拟地址 -> pte_index 页表项索引值
#define VMM_PTE_INDEX(vaddr) (((uint32_t)(vaddr) >> 12) & 0x3FF)

// 页目录页表相关标志位
enum vmm_flag {
  VMM_PRESENT = BIT(0),
  VMM_WRITABLE = BIT(1),
  VMM_USER = BIT(2),
  VMM_PG_4M = BIT(7),
};

// 页目录项
typedef uint32_t pde_t;

// 页表项
typedef uint32_t pte_t;

// 页目录
struct page_directory {
  pde_t entries[PAGES_PER_DIR];
};

// 页表
struct page_table {
  pte_t entries[PAGES_PER_TABLE];
};

// 内核页目录
extern struct page_directory *kernel_pgdir;
extern phys_addr_t kernel_pgdir_phys;

// 初始化虚拟内存管理
int vmm_setup();

// 在当前地址空间下建立页映射
int vmm_map_page(uintptr_t vaddr, phys_addr_t paddr, uint32_t flags);

// 在当前地址空间下取消页映射
void vmm_unmap_page(uintptr_t vaddr);

// 在当前地址空间下根据虚拟地址获得其映射对应的物理地址
int vmm_v2p(uintptr_t vaddr, phys_addr_t *p_paddr);

// 切换地址空间（修改 cr3 寄存器）
void vmm_switch_pgdir(phys_addr_t pgdir);

#endif  // !_NOS_MM_VMM_H
