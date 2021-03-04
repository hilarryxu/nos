#ifndef _NOS_MM_MEMLAYOUT_H
#define _NOS_MM_MEMLAYOUT_H

extern char _kernel_end[];

// 内核基址
#define KERNEL_BASE 0
#define KERNEL_VIRTUAL_START 0xC0000000

#define P2V(paddr) ((paddr) + KERNEL_BASE)
#define V2P(vaddr) ((vaddr)-KERNEL_BASE)

#define CAST_P2V(addr) (void *)P2V((addr))
#define CAST_V2P(addr) V2P((phys_addr_t)(addr))

// 页大小 4KB
#define PAGE_SIZE 0x1000

// 页掩码，用于 4KB 对齐
#define PAGE_MASK 0xFFFFF000

// 内核页目录存放位置
#define KERNEL_PGDIR 0x1000
// 内核堆栈栈底
#define KERNEL_STACK 0x9FC00

// [1MB, 4MB) 为内核可执行文件加载至内存区域

// 内核的 4MB 页表存放位置 [4MB, 8MB)
#define KERNEL_PG_1 0x400000
#define KERNEL_PG_1_LIM 0x800000

// [0, 8MB) 直接对等映射
#define NR_IDENTITY_MAP 2

// [3.5G, 4G) 内核堆
#define KERNEL_HEAP_START 0xE0000000
#define KERNEL_HEAP_END 0xFFFFFFFF

#endif  // !_NOS_MM_MEMLAYOUT_H
