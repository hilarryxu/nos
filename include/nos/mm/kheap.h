#ifndef _NOS_MM_KHEAP_H
#define _NOS_MM_KHEAP_H

#include <stddef.h>
#include <stdint.h>

// 初始化内核堆管理
void kheap_setup();

// 内核堆分配
void *kmalloc(size_t size);

// 内核堆释放
void kfree(void *p);

// 按页对齐分配 2^order 页面
void *kmalloc_ap(uint32_t order);

// 按页对齐释放
void kfree_ap(void *p, uint32_t order);

#endif  // _NOS_MM_KHEAP_H
