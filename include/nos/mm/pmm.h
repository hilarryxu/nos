#ifndef _NOS_MM_PMM_H
#define _NOS_MM_PMM_H

#include <stdint.h>

// 256MB
#define PMM_MAX_MEM_SIZE 0x10000000
// 4KB
#define PMM_PAGE_SIZE 0x1000

#define PMM_MAX_PAGE (PMM_MAX_MEM_SIZE / PMM_PAGE_SIZE)

void pmm_setup();

uint32_t pmm_alloc();
void pmm_free(uint32_t pa);

#endif  // !_NOS_MM_PMM_H
