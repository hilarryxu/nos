#include <nos/mm/vmm.h>

#include <stdint.h>

#include <nos/mm/pmm.h>
#include <nos/mm/kheap.h>

// static struct page_directory *current_pgdir;

struct page_directory *
vmm_alloc_vaddr_space()
{
  struct page_directory *page_dir = kmalloc(sizeof(*page_dir));

  if (page_dir) {
    for (int i = 0; i < PAGES_PER_DIR; ++i)
      page_dir->entries[i] = VMM_WRITABLE;
  }

  return page_dir;
}

void
vmm_free_vaddr_space(struct page_directory *page_dir)
{
  kfree(page_dir);
}

int
vmm_map_page(struct page_directory *page_dir, uintptr_t vaddr,
             phys_addr_t paddr, uint32_t flags)
{
  uint32_t pde_index = VMM_PDE_INDEX(vaddr);
  uint32_t pte_index = VMM_PTE_INDEX(vaddr);
  struct page_table *page_table;
  int i;

  if ((vaddr & 0xFFF) || (paddr & 0xFFF)) {
    return -1;
  }

  if (page_dir->entries[pde_index] & VMM_PRESENT) {
    page_table =
        (struct page_table *)(page_dir->entries[pde_index] & PAGE_MASK);
  } else {
    page_table = (struct page_table *)kmalloc(sizeof(*page_table));
    for (i = 0; i < PAGES_PER_TABLE; i++) {
      page_table->entries[i] = 0;
    }
    page_dir->entries[pte_index] =
        (pde_t)page_table | VMM_WRITABLE | VMM_PRESENT;
  }

  page_table->entries[pte_index] = (pte_t)paddr | flags | VMM_PRESENT;

  asm volatile("invlpg %0" : : "m"(*(char *)vaddr));

  return 0;
}

void
vmm_setup()
{
}
