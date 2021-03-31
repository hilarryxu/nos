#include <nos/mm/kheap.h>

#include <nos/mm/pmm.h>
#include <nos/libs/list.h>

#define SIZE_ALIGNMENT NOS_ALIGNMENT

struct kmem_cache;

// slab 页
struct slab_page {
  struct list_entry list;
  void *s_mem;
  unsigned int inuse;
  unsigned int free;
};

// slab 分配器高速缓存
struct kmem_cache {
  unsigned int size;     // 对象长度 + 填充字节
  unsigned int perslab;  // 一页中能存放的最大对象个数

  struct list_entry slabs_partial;  // 部分使用的 slab 描述符
  struct list_entry slabs_full;     // 使用完的 slab 描述符
  struct list_entry slabs_free;     // 完全空闲的 slab 描述符
};

#define NR_KHEAP_CACHES 9
static struct kmem_cache kheap_caches[NR_KHEAP_CACHES];

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
    list_init(&cache->slabs_partial);
    list_init(&cache->slabs_full);
    list_init(&cache->slabs_free);
  }

  log_debug(LOG_INFO, "[kheap] done");

  return NOS_OK;
}

static void *
alloc_object(struct kmem_cache *cache)
{
  UNUSED(cache);

  void *object = NULL;

  object = alloc_object_from_slab_list(&cache->slabs_partial);
  if (!object)
    object = alloc_object_from_slab_list(&cache->slabs_free);

  if (!object) {
    struct slab_page *slab = slab_page_alloc(cache);
    object = alloc_object_from_slab(slab);
    if (!object)
      log_panic("failed to alloc object from a new slab");
  }

  return object;
}

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

  return alloc_object(cache);
}

//---------------------------------------------------------------------
// 内核堆释放
//---------------------------------------------------------------------
void
kfree(void *p)
{
  UNUSED(p);
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
