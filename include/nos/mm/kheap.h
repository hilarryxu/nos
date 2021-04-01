#ifndef _NOS_MM_KHEAP_H
#define _NOS_MM_KHEAP_H

//=====================================================================
// 内核堆管理
//
// 有如下特点：
//   分配的空间物理地址连续
//   利用了预先映射好的页映射
//   虚拟地址 = 物理地址 + KERNEL_BASE
//
// 分配速度相对较块，不用修改页映射，不用管 TLB 缓存。
//=====================================================================

#include <stddef.h>
#include <stdint.h>

struct kmem_cache;

struct kmem_cache *kmem_cache_create(const char *name, size_t object_size,
                                     size_t align, unsigned long flags);

void kmem_cache_destroy(struct kmem_cache *cache);

void *kmem_cache_alloc(struct kmem_cache *cache);

void kmem_cache_free(struct kmem_cache *cache, void *object);

// 初始化内核堆管理
int kheap_setup();

// 内核堆分配
void *kmalloc(size_t size);

// 内核堆释放
void kfree(void *p);

// 按页对齐分配 npage 个页面
void *kmalloc_ap(int npage);

// 按页对齐释放
void kfree_ap(void *p, int npage);

#endif  // !_NOS_MM_KHEAP_H
