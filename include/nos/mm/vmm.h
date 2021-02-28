#ifndef _NOS_MM_VMM_H
#define _NOS_MM_VMM_H

#include <nos/macros.h>
#include <nos/mm/memlayout.h>

#define NR_PDE 1024
#define NR_PTE 1024

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
  pde_t entries[NR_PDE];
};

struct page_table {
  pte_t entries[NR_PTE];
};

struct vmm_context {
  struct page_directory *page_dir;
  phys_addr_t page_dir_phys;
};

void vmm_setup();

#endif  // !_NOS_MM_VMM_H
