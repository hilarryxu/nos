#ifndef _NOS_MM_KHEAP_H
#define _NOS_MM_KHEAP_H

#include <stddef.h>

// 初始化内核堆管理
void kheap_setup();

// 内核堆分配
void *kmalloc(size_t size);

// 内核堆释放
void kfree(void *p);

#endif  // _NOS_MM_KHEAP_H
