#ifndef _NOS_MM_MEMLAYOUT_H
#define _NOS_MM_MEMLAYOUT_H

// 内核基址
#define KERNEL_BASE 0

// 页大小 4KB
#define PAGE_SIZE 0x1000

// 内核页目录存放位置
#define KERNEL_PGDIR 0x1000
// 内核堆栈栈底
#define KERNEL_STACK 0x9FC00

// [1MB, 4MB) 为内核可执行文件加载至内存区域

// 内核的 4MB 页表存放位置 [4MB, 8MB)
#define KERNEL_PG_1 0x400000
#define KERNEL_PG_1_LIM 0x800000

// 直接物理内存分配区域 [8MB, 16MB)
#define PLACEMENT_START 0x800000
#define PLACEMENT_END 0x1000000

// [0, 16MB) 直接对等映射
#define NR_IDENTITY_MAP 4

#endif  // !_NOS_MM_MEMLAYOUT_H
