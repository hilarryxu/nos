#ifndef _NOS_MM_VADDR_SPACE_H
#define _NOS_MM_VADDR_SPACE_H

#include <nos/nos.h>

struct page_directory;

// 创建一个新的地址空间
struct page_directory *vaddr_space_create(phys_addr_t *p_pgdir_phys);

// 销毁地址空间
int vaddr_space_destroy(struct page_directory *vaddr_space);

// 拷贝内核地址空间
void copy_kernel_space(struct page_directory *vaddr_space);

#endif  // _NOS_MM_VADDR_SPACE_H
