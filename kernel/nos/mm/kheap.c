#include <nos/mm/kheap.h>

#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

#define SIZE_ALIGNMENT NOS_ALIGNMENT

struct kheap_allocator {
  char *start_addr;         // 起始地址
  char *free_addr;          // 堆空闲地址
  char *allocated_end_ptr;  // 按页对齐已分配的结尾地址
};

static struct kheap_allocator allocator;

void
kheap_setup()
{
  allocator.start_addr = (char *)KERNEL_HEAP_BEGIN;
  allocator.free_addr = allocator.start_addr;
  allocator.allocated_end_ptr = allocator.start_addr;
}

static int
kheap_expand(char *end_addr)
{
  char *vaddr;
  phys_addr_t paddr;
  char *start_addr = allocator.allocated_end_ptr;

  // 多分配一页，处理边界情况
  end_addr = PTR_ALIGN_UP(end_addr, PAGE_SIZE) + PAGE_SIZE;

  for (vaddr = start_addr; vaddr < end_addr; vaddr += PAGE_SIZE) {
    // 分配物理页框，并在当前地址空间下（内核空间范围内）增加映射
    paddr = pmm_alloc_block();
    // 由于内核堆的虚拟地址在内核地址空间下
    // 会修改共享给所有用户进程的内核页表
    // 达到一处修改，其他所有进程都可以看到（内核页表是共享的）
    vmm_map_page((uintptr_t)vaddr, paddr, VMM_WRITABLE);
    allocator.allocated_end_ptr += PAGE_SIZE;
  }

  return NOS_OK;
}

void *
kmalloc(size_t size)
{
  int rc;

  if (size == 0)
    return NULL;

  char *free_addr = PTR_ALIGN_UP(allocator.free_addr, SIZE_ALIGNMENT);
  size_t aligned_size = ALIGN_UP(size, SIZE_ALIGNMENT);
  // 剩余空间不够，扩展堆
  if (free_addr + aligned_size >= allocator.allocated_end_ptr) {
    rc = kheap_expand(free_addr + aligned_size);
    if (rc != NOS_OK)
      return NULL;
  }

  allocator.free_addr = free_addr + aligned_size;

  ASSERT((uintptr_t)free_addr > KERNEL_BASE);
  return free_addr;
}

// TODO: 待实现
void
kfree(void *p)
{
  UNUSED(p);
}
