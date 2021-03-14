#ifndef _NOS_MM_PMM_H
#define _NOS_MM_PMM_H

#include <stdint.h>

#include <nos/nos.h>
#include <nos/mm/memlayout.h>

// 支持的最大内存 256MB
#define PMM_MAX_MEM_SIZE 0x10000000

// 物理页框大小（一般和页大小一样）
#define PMM_FRAME_SIZE PAGE_SIZE

// 最大内存下的物理页框个数
#define PMM_MAX_FRAME (PMM_MAX_MEM_SIZE / PMM_FRAME_SIZE)

// 初始化物理内存管理子系统
int pmm_setup(phys_addr_t free_addr, struct multiboot_info *mb_info);

// 申请分配一个物理页框
phys_addr_t pmm_alloc_block();

// 释放物理地址对应的页框
void pmm_free_block(phys_addr_t paddr);

// 获取总的可用物理页框数
uint32_t pmm_get_total_frames();

// 获取最大物理内存地址
uint64_t pmm_get_max_phys_addr(struct multiboot_info *mb_info);

#endif  // _NOS_MM_PMM_H
