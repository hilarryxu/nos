#ifndef _NOS_MM_PAGING_H
#define _NOS_MM_PAGING_H

struct page_directory;
extern struct page_directory *kernel_pgdir;

void paging_setup();

void paging_copy_kernel_space(struct page_directory *vaddr_space);

#endif  // !_NOS_MM_PAGING_H
