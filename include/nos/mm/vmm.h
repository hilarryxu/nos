#ifndef _NOS_MM_VMM_H
#define _NOS_MM_VMM_H

#include <nos/macros.h>
#include <nos/mm/memlayout.h>

#define PAGES_PER_DIR 1024
#define PAGES_PER_TABLE 1024

#define VMM_PDE_INDEX(vaddr) (((uint32_t)(vaddr) >> 22) & 0x3FF)

#define VMM_PTE_INDEX(vaddr) (((uint32_t)(vaddr) >> 12) & 0x3FF)

enum vmm_flag {
  VMM_PRESENT = 0x1,
  VMM_WRITABLE = 0x2,
  VMM_USER = 0x4,
};

typedef uint32_t pde_t;
typedef uint32_t pte_t;

struct page_directory {
  pde_t entries[PAGES_PER_DIR];
};

struct page_table {
  pte_t entries[PAGES_PER_TABLE];
};

struct vmm_context {
  struct page_directory *page_dir;
  phys_addr_t page_dir_phys;
};

void vmm_setup();

void vmm_activate_pgdir(phys_addr_t pgdir);

#endif  // !_NOS_MM_VMM_H
