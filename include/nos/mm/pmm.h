#ifndef _NOS_MM_PMM_H
#define _NOS_MM_PMM_H

#include <stdint.h>

#include <nos/macros.h>
#include <nos/mm/memlayout.h>

// 256MB
#define PMM_MAX_MEM_SIZE 0x10000000

#define PMM_MAX_PAGE (PMM_MAX_MEM_SIZE / PAGE_SIZE)

void pmm_setup();

phys_addr_t pmm_alloc();
void pmm_free(phys_addr_t pa);

#endif  // !_NOS_MM_PMM_H
