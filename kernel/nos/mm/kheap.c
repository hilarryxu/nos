#include <nos/mm/kheap.h>

#include <nos/mm/paging.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>

#define SIZE_ALIGNMENT NOS_ALIGNMENT

struct kheap_allocator {
  char *start_addr;
  char *free_addr;
  char *allocated_end_ptr;
};

static struct kheap_allocator allocator;

void
kheap_setup()
{
  allocator.start_addr = (char *)KERNEL_HEAP_START;
  allocator.free_addr = allocator.start_addr;
  allocator.allocated_end_ptr = allocator.start_addr;
}

static int
kheap_expand(char *end_addr)
{
  char *start_addr = allocator.allocated_end_ptr;
  end_addr = PTR_ALIGN_UP(end_addr, PAGE_SIZE) + PAGE_SIZE;
  char *vaddr;
  phys_addr_t paddr;

  for (vaddr = start_addr; vaddr < end_addr; vaddr += PAGE_SIZE) {
    paddr = pmm_alloc_block();
    vmm_map_page((uintptr_t)vaddr, paddr, VMM_WRITABLE);
  }

  return 0;
}

void *
kmalloc(size_t size)
{
  int rc;
  if (size == 0)
    return NULL;

  char *free_addr = PTR_ALIGN_UP(allocator.free_addr, SIZE_ALIGNMENT);
  size_t aligned_size = ALIGN_UP(size, SIZE_ALIGNMENT);
  if (free_addr + aligned_size >= allocator.allocated_end_ptr) {
    rc = kheap_expand(free_addr + aligned_size);
    if (rc != 0) {
      return NULL;
    }
  }

  allocator.free_addr = free_addr + aligned_size;
  return free_addr;
}

void
kfree(void *p)
{
  (void)p;
}
