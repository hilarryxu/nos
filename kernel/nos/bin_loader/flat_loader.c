#include <nos/bin_loader/loader.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/proc/process.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vaddr_space.h>

#define BINARY_ENTRY_VADDR 0x200000

static inline phys_addr_t
read_cr3()
{
  phys_addr_t cr3;
  asm volatile("mov %%cr3, %0" : "=r"(cr3)::"memory");
  return cr3;
}

static int
load_binary_program(struct process *process, uintptr_t image_start,
                    size_t image_size)
{
  size_t aligned_size = ALIGN_UP(image_size, PAGE_SIZE);
  uintptr_t vaddr_start = (uintptr_t)BINARY_ENTRY_VADDR;
  uintptr_t vaddr = vaddr_start;
  uintptr_t end_vaddr = vaddr + aligned_size;

  for (; vaddr < end_vaddr; vaddr += PAGE_SIZE) {
    phys_addr_t paddr = pmm_alloc_block();
    vmm_map_page(vaddr, paddr, VMM_WRITABLE | VMM_USER);
  }

  bzero((void *)vaddr_start, aligned_size);
  memcpy((void *)vaddr_start, (void *)image_start, image_size);

  process->entry = (void *)vaddr_start;

  return NOS_OK;
}

int
loader_load_flat(struct process *process, uintptr_t image_start,
                 size_t image_size)
{
  int rc = NOS_OK;
  phys_addr_t prev_cr3 = read_cr3();

  // 给进程创建地址空间
  process->pgdir = vaddr_space_create(&process->cr3);
  if (!process->pgdir)
    return -1;

  // 切换到新建进程的地址空间
  vmm_switch_pgdir(process->cr3);

  // 加载二进制可执行程序到进程的地址空间
  if (load_binary_program(process, image_start, image_size) != NOS_OK) {
    rc = -1;
    goto bad;
  }

  // 给进程分配内核栈和用户栈
  if (alloc_process_stacks(process) != NOS_OK) {
    rc = -1;
    goto bad;
  }

bad:
  // 切换回原来的地址空间
  vmm_switch_pgdir(prev_cr3);

  return rc;
}
