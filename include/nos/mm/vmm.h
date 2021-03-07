#ifndef _NOS_MM_VMM_H
#define _NOS_MM_VMM_H

#include <nos/macros.h>
#include <nos/mm/memlayout.h>

#define PAGES_PER_DIR 1024
#define PAGES_PER_TABLE 1024

#define VMM_PDE_INDEX(vaddr) (((uint32_t)(vaddr) >> 22) & 0x3FF)

#define VMM_PTE_INDEX(vaddr) (((uint32_t)(vaddr) >> 12) & 0x3FF)

#define VMM_INVALIDATE_PAGE(v_addr) asm("invlpg %0" ::"m"(v_addr))

enum vmm_flag {
  VMM_PRESENT = BIT(0),
  VMM_WRITABLE = BIT(1),
  VMM_USER = BIT(2),
  VMM_PG_4M = BIT(7),
};

typedef uint32_t pde_t;
typedef uint32_t pte_t;

struct page_directory {
  pde_t entries[PAGES_PER_DIR];
};

struct page_table {
  pte_t entries[PAGES_PER_TABLE];
};

extern struct page_directory *kernel_pgdir;

void vmm_setup();

struct page_directory *vmm_alloc_vaddr_space();

void vmm_map_page(uintptr_t vaddr, phys_addr_t paddr, uint32_t flags);
void vmm_unmap_page(uintptr_t vaddr);

struct page_directory *vaddr_space_create(phys_addr_t *p_pgdir_phys);

// 切换地址空间
void vmm_switch_pgdir(phys_addr_t pgdir);

#endif  // _NOS_MM_VMM_H
