#include <nos/mm/kheap.h>

#include <nos/mm/pmm.h>
#include <nos/libs/list.h>

#define VADDR_TO_PAGE(vaddr) (void *)((uintptr_t)(vaddr) & ~(PAGE_SIZE - 1))

// slab 页面
struct slab {
  struct list_entry slab_link;
  struct kmem_cache *cache;
  char *objects;
  size_t nr_free;
  void *free[0];
};

#define PAYLOAD_SIZE (PAGE_SIZE - sizeof(struct slab))
#define slab_to_list_entry(slab) (struct list_entry *)((slab))
#define list_entry_to_slab(entry) (struct slab *)((entry))

// slab 分配器高速缓存
struct kmem_cache {
  size_t object_size;    // 对象大小
  size_t align;          // 对象对齐字节
  size_t size;           // 对象大小 + 填充字节
  unsigned int perslab;  // 一页中能存放的最大对象个数

  struct list_entry slabs_full;     // 使用完的 slab 描述符
  struct list_entry slabs_partial;  // 部分使用的 slab 描述符
  struct list_entry slabs_empty;    // 完全空闲的 slab 描述符
};

#define NR_KHEAP_CACHES 9
static struct kmem_cache kheap_caches[NR_KHEAP_CACHES];

static void
slab_init(struct slab *slab, struct kmem_cache *cache)
{
  slab->cache = cache;
  slab->nr_free = 0;
  slab->objects = (char *)slab + (PAGE_SIZE - (cache->perslab * cache->size));

  for (size_t i = 0; i < cache->perslab; i++) {
    slab->free[slab->nr_free++] = slab->objects + (i * cache->size);
  }

  list_add_before(&cache->slabs_empty, slab_to_list_entry(slab));
}

static struct slab *
alloc_slab(struct kmem_cache *cache)
{
  struct slab *slab = NULL;

  phys_addr_t page = pmm_alloc_block();
  if (!page)
    log_panic("failed to alloc a physical page");

  slab = CAST_P2V(page);
  slab_init(slab, cache);

  return slab;
}

static void *
alloc_object_from_slab(struct slab *slab)
{
  void *object = NULL;

  if (slab->nr_free == 0)
    log_panic("alloc object from slabs_full");

  object = slab->free[--slab->nr_free];

  if (slab->nr_free == 0) {
    // slabs_partial -> slabs_full
    list_del(slab_to_list_entry(slab));
    list_add_before(&slab->cache->slabs_full, slab_to_list_entry(slab));
  } else if (slab->nr_free + 1 == slab->cache->perslab) {
    // slabs_empty -> slabs_partial
    list_del(slab_to_list_entry(slab));
    list_add_before(&slab->cache->slabs_partial, slab_to_list_entry(slab));
  }

  return object;
}

static void *
alloc_object_from_slab_list(struct list_entry *head)
{
  if (list_is_empty(head))
    return NULL;

  return alloc_object_from_slab(list_entry_to_slab(list_next(head)));
}

//---------------------------------------------------------------------
// 初始化内核堆管理
//---------------------------------------------------------------------
int
kheap_setup()
{
  log_debug(LOG_INFO, "[kheap] setup");

  struct kmem_cache *cache;
  for (size_t i = 0; i < NR_KHEAP_CACHES; i++) {
    cache = &kheap_caches[i];
    cache->size = 2 << (i + 1);
    cache->object_size = cache->size;
    cache->align = NOS_ALIGNMENT;
    cache->perslab = PAYLOAD_SIZE / (cache->size + sizeof(void *));

    list_init(&cache->slabs_full);
    list_init(&cache->slabs_partial);
    list_init(&cache->slabs_empty);
  }

  log_debug(LOG_INFO, "[kheap] done");

  return NOS_OK;
}

//---------------------------------------------------------------------
// 从 kmem_cache 中分配一个对象
//---------------------------------------------------------------------
void *
kmem_cache_alloc(struct kmem_cache *cache)
{
  void *object = NULL;

  // 先从 slabs_partial 中分配，失败再从 slabs_empty 中分配
  object = alloc_object_from_slab_list(&cache->slabs_partial);
  if (!object)
    object = alloc_object_from_slab_list(&cache->slabs_empty);

  // 还是失败就新分配一个 slab 页面
  if (!object) {
    struct slab *slab = alloc_slab(cache);
    object = alloc_object_from_slab(slab);
    if (!object)
      log_panic("failed to alloc object from a new slab");
  }

  return object;
}

//---------------------------------------------------------------------
// 释放对象归还到 kmem_cache 中去
//---------------------------------------------------------------------
void
kmem_cache_free(struct kmem_cache *cache, void *object)
{
  struct slab *slab = VADDR_TO_PAGE(object);

  if (slab->cache != cache)
    log_panic("free object in wrong kmem_cache");

  slab->free[slab->nr_free++] = object;

  if (slab->nr_free == 1 && slab->nr_free < slab->cache->perslab) {
    // slabs_full -> slabs_partial
    list_del(slab_to_list_entry(slab));
    list_add_before(&slab->cache->slabs_partial, slab_to_list_entry(slab));
  } else if (slab->nr_free == slab->cache->perslab) {
    // slabs_partial -> slabs_empty
    list_del(slab_to_list_entry(slab));
    list_add_before(&slab->cache->slabs_empty, slab_to_list_entry(slab));
  }
}

//---------------------------------------------------------------------
// 根据对象大小找到对应堆缓存索引下标
//---------------------------------------------------------------------
static inline size_t
get_kheap_cache_index(size_t size)
{
  if (size <= 4)
    return 0;
  if (size <= 8)
    return 1;
  if (size <= 16)
    return 2;
  if (size <= 32)
    return 3;
  if (size <= 64)
    return 4;
  if (size <= 128)
    return 5;
  if (size <= 256)
    return 6;
  if (size <= 512)
    return 7;
  if (size <= 1024)
    return 8;

  log_panic("[BUG] kmalloc: can not alloc memory for more than 1024");
  return 0;
}

//---------------------------------------------------------------------
// 内核堆分配
//---------------------------------------------------------------------
void *
kmalloc(size_t size)
{
  size_t index = get_kheap_cache_index(size);
  struct kmem_cache *cache = &kheap_caches[index];
  return kmem_cache_alloc(cache);
}

//---------------------------------------------------------------------
// 内核堆释放
//---------------------------------------------------------------------
void
kfree(void *p)
{
  struct slab *slab = VADDR_TO_PAGE(p);
  kmem_cache_free(slab->cache, p);
}

//---------------------------------------------------------------------
// 按页对齐分配 npage 个页面
//---------------------------------------------------------------------
void *
kmalloc_ap(int npage)
{
  UNUSED(npage);

  return kmalloc(npage * PAGE_SIZE);
}

//---------------------------------------------------------------------
// 按页对齐释放
//---------------------------------------------------------------------
void
kfree_ap(void *p, int npage)
{
  UNUSED(npage);

  kfree(p);
}
