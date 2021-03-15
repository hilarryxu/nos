#ifndef _NOS_MM_PAGING_H
#define _NOS_MM_PAGING_H

#include <nos/types.h>

struct multiboot_info;

// 初始化分页
phys_addr_t paging_setup(phys_addr_t page_aligned_free,
                         struct multiboot_info *mb_info);

#endif  // !_NOS_MM_PAGING_H
