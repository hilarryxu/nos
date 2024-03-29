#ifndef _NOS_MM_MEMLAYOUT_H
#define _NOS_MM_MEMLAYOUT_H

extern char KERNEL_START_PHYS[];
extern char KERNEL_END_PHYS[];
extern char KERNEL_START[];
extern char KERNEL_END[];
extern char KERNEL_SIZE[];

extern char KERNEL_TEXT_START[];
extern char KERNEL_TEXT_END[];
extern char KERNEL_DATA_START[];
extern char KERNEL_DATA_END[];
extern char KERNEL_RODATA_START[];
extern char KERNEL_RODATA_END[];
extern char KERNEL_BSS_START[];
extern char KERNEL_BSS_END[];

// 内核基址
#define KERNEL_BASE 0xC0000000
#define KERNEL_PDE_INDEX (KERNEL_BASE >> 22)

#define P2V(paddr) ((paddr) + (KERNEL_BASE))
#define V2P(vaddr) ((vaddr) - (KERNEL_BASE))

#define CAST_P2V(addr) (void *)P2V((addr))
#define CAST_V2P(addr) V2P((phys_addr_t)(addr))

#define SIZE_4MB 0x400000

// 页大小 4KB
#define PAGE_SIZE 0x1000

// 取得页框号的右移位数
#define PAGE_SHIFT 12

// 页掩码，用于 4KB 对齐
#define PAGE_FRAME_MASK 0xFFFFF000

// 页标志掩码
#define PAGE_FLAG_MASK 0xFFF

// 内核堆栈栈底
#define KERNEL_STACK 0x9FC00

#define KERNEL_STACK_NPAGE 1

// 内核栈大小
#define KERNEL_STACK_SIZE (KERNEL_STACK_NPAGE * PAGE_SIZE)

// [1MB, 4MB) 为内核可执行文件加载至内存区域

// 内核的 4MB 页表存放位置 [4MB, 8MB)
#define KERNEL_PG_1 0x400000
#define KERNEL_PG_1_LIM 0x800000

// [0, 16MB) 直接对等映射
#define NR_IDENTITY_MAP 4

// [3G + 12MB, 3G + 16MB) 的虚拟空间留给用户进程的页目录表
// 也就是最多支持 1024 个用户进程
#define PGDIR_AREA_BEGIN (KERNEL_BASE + 0x00C00000)
#define PGDIR_AREA_END (KERNEL_BASE + 0x01000000)

// [3G + 32MB, 3G + 256MB) 内核堆
#define KERNEL_HEAP_BEGIN 0xC2000000
#define KERNEL_HEAP_END 0xD0000000

// 默认可供分配使用的物理地址范围 [16MB, 32MB)
#define DEFAULT_PHYS_ADDR_START 0x1000000
#define DEFAULT_PHYS_ADDR_END 0x2000000

#endif  // !_NOS_MM_MEMLAYOUT_H
