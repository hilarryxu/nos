#include <nos/mm/vmm.h>

#include <stdint.h>

#include <nos/mm/pmm.h>

static struct vmm_context *kernel_vmm_context;

struct vmm_context *
vmm_create_context()
{
  struct vmm_context *ctx = (struct vmm_context *)pmm_alloc();

  ctx->page_dir = (struct page_directory *)pmm_alloc();
  ctx->page_dir_phys = (phys_addr_t)ctx->page_dir;

  for (int i = 0; i < NR_PDE; i++) {
    ctx->page_dir->entries[i] = 0;
  }

  return ctx;
}

void
vmm_activate_context(struct vmm_context *context)
{
  asm volatile("mov %0, %%cr3" : : "r"(context->page_dir_phys));
}

int
vmm_map_page(struct vmm_context *context, uintptr_t vaddr, phys_addr_t paddr,
             uint32_t flags)
{
  struct page_directory *page_dir = context->page_dir;
  uint32_t pde_index = VMM_PDE_INDEX(vaddr);
  uint32_t pte_index = VMM_PTE_INDEX(vaddr);
  struct page_table *page_table;
  int i;

  if ((vaddr & 0xFFF) || (paddr & 0xFFF)) {
    return -1;
  }

  if (page_dir->entries[pde_index] & VMM_PRESENT) {
    page_table = (struct page_table *)(page_dir->entries[pde_index] & ~0xFFF);
  } else {
    page_table = (struct page_table *)pmm_alloc();
    for (i = 0; i < NR_PTE; i++) {
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
  uint32_t cr0;

  kernel_vmm_context = vmm_create_context();

  for (int i = 0; i < 1024 * PAGE_SIZE; i += PAGE_SIZE) {
    vmm_map_page(kernel_vmm_context, i, i, VMM_WRITABLE);
  }

  vmm_activate_context(kernel_vmm_context);

  // WOW: 开启分页机制
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= (1 << 31);
  asm volatile("mov %0, %%cr0" : : "r"(cr0));
}
