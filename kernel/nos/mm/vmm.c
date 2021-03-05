#include <nos/mm/vmm.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <nos/nos.h>
#include <nos/mm/pmm.h>
#include <nos/mm/kheap.h>

#define MAGIC_PAGE_DIR_ADDR 0xFFFFF000
#define MAGIC_PAGE_TABLE_ADDR 0xFFC00000

// static struct page_directory *current_pgdir;

enum vmm_get_pte_ptr_op {
  VMM_CREATE_PAGE_TABLE_E = 0,
  VMM_NOT_CREATE_PAGE_TABLE_E
};

pte_t *
vmm_get_pte_ptr(uintptr_t vaddr, enum vmm_get_pte_ptr_op op, uint32_t flags)
{
  if (vaddr & 0xFFF) {
    log_panic("vmm_get_pte_ptr: unaligned vaddr 0x%X", vaddr);
  }

  uint32_t pde_index = VMM_PDE_INDEX(vaddr);
  uint32_t pte_index = VMM_PTE_INDEX(vaddr);

  struct page_directory *page_dir =
      (struct page_directory *)MAGIC_PAGE_DIR_ADDR;
  struct page_table *page_table =
      (struct page_table *)(MAGIC_PAGE_TABLE_ADDR + (pde_index << 12));

  if (!(page_dir->entries[pde_index] & VMM_PRESENT) &&
      (op == VMM_CREATE_PAGE_TABLE_E)) {
    phys_addr_t page_table_phys = pmm_alloc_block();
    page_dir->entries[pte_index] = (pde_t)page_table_phys | flags | VMM_PRESENT;
    bzero((void *)page_table, PAGE_SIZE);
  }

  if (page_dir->entries[pde_index] & VMM_PRESENT) {
    return &(page_table->entries[pte_index]);
  }
  return NULL;
}

void
vmm_map_page(uintptr_t vaddr, phys_addr_t paddr, uint32_t flags)
{
  pte_t *pte_ptr =
      vmm_get_pte_ptr(vaddr, VMM_CREATE_PAGE_TABLE_E, VMM_WRITABLE);

  if (*pte_ptr & VMM_PRESENT) {
    log_panic("vmm_map_page: failed to map 0x%X to 0x%X", vaddr, paddr, vaddr,
              *pte_ptr & PAGE_MASK);
  }

  *pte_ptr = (pte_t)paddr | flags | VMM_PRESENT;

  // no 4MB pages
  VMM_INVALIDATE_PAGE(vaddr);
}

void
vmm_unmap_page(uintptr_t vaddr)
{
  pte_t *pte_ptr = vmm_get_pte_ptr(vaddr, VMM_NOT_CREATE_PAGE_TABLE_E, 0);

  if (pte_ptr) {
    pmm_free_block(*pte_ptr & PAGE_MASK);
    *pte_ptr = 0;
  }
}

void
vmm_map_pages(uintptr_t vaddr, phys_addr_t paddr, size_t num, uint32_t flags)
{
  for (size_t i = 0; i < num; i++) {
    vmm_map_page(vaddr, paddr, flags);
    vaddr += PAGE_SIZE;
    paddr += PAGE_SIZE;
  }
}

void
vmm_unmap_pages(uintptr_t vaddr, size_t num)
{
  for (size_t i = 0; i < num; i++) {
    vmm_unmap_page(vaddr);
    vaddr += PAGE_SIZE;
  }
}

int
vmm_v2p(uintptr_t vaddr, phys_addr_t *paddr_ptr)
{
  pte_t *pte_ptr = vmm_get_pte_ptr(vaddr, VMM_NOT_CREATE_PAGE_TABLE_E, 0);

  if (!pte_ptr)
    return -1;

  if (paddr_ptr) {
    return (((uintptr_t)*pte_ptr) & PAGE_MASK) + (vaddr & 0xFFF);
  }

  return NOS_OK;
}

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
old_vmm_map_page(struct page_directory *page_dir, uintptr_t vaddr,
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
