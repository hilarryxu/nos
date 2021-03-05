#ifndef _NOS_MM_PAGING_H
#define _NOS_MM_PAGING_H

#include <nos/nos.h>

struct page_directory;
extern struct page_directory *kernel_pgdir;

void paging_setup();

void paging_copy_kernel_space(struct page_directory *vaddr_space);

void paging_switch_pgdir(phys_addr_t pgdir);

#endif  // !_NOS_MM_PAGING_H
