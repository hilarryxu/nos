#include <nos/bin_loader/loader.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/elf.h>
#include <nos/proc/process.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vaddr_space.h>

static inline phys_addr_t
read_cr3()
{
  phys_addr_t cr3;
  asm volatile("mov %%cr3, %0" : "=r"(cr3)::"memory");
  return cr3;
}

static int
load_from_prog_header(uintptr_t image_start, size_t image_size,
                      struct elf_prog_header *ph, struct process *proc)
{
  size_t file_size = ph->p_filesz;
  size_t file_offset = ph->p_offset;
  size_t page_offset = ph->p_vaddr & (PAGE_SIZE - 1);
  uintptr_t vaddr_end = ph->p_vaddr + ph->p_memsz;

  if (ph->p_memsz < ph->p_filesz)
    return -1;

  if (ph->p_offset >= image_size || ph->p_offset + ph->p_filesz > image_size)
    return -1;

  if (ph->p_memsz == 0)
    return NOS_OK;

  for (uintptr_t vaddr = ph->p_vaddr - page_offset; vaddr < vaddr_end;
       vaddr += PAGE_SIZE) {
    // 当前页下需要拷贝的大小
    size_t sz_in_page = PAGE_SIZE - page_offset;
    phys_addr_t paddr = pmm_alloc_block();
    void *dst = CAST_P2V(paddr + page_offset);

    if (!paddr)
      return -1;

    bzero(CAST_P2V(paddr), PAGE_SIZE);

    if (vmm_map_page(proc->pgdir, vaddr, paddr, VMM_WRITABLE | VMM_USER) < 0) {
      pmm_free_block(paddr);
      return -1;
    }

    if (file_size != 0) {
      size_t need_copy_sz = MIN(file_size, sz_in_page);
      memcpy(dst, (void *)(image_start + file_offset), need_copy_sz);
      file_offset += need_copy_sz;
      file_size -= need_copy_sz;
    }

    page_offset = 0;
  }

  return NOS_OK;
}

static int
load_from_prog_headers(uintptr_t image_start, size_t image_size,
                       struct elf_prog_header *ph, uint32_t ph_struct_size,
                       uint32_t num, struct process *proc)
{
  for (uint32_t i = 0; i < num; i++, ph += ph_struct_size) {
    loga("num=%d, [%d] type=%d", num, i, ph->p_type);
    if (ph->p_type == ELF_PROG_LOAD) {
      if (load_from_prog_header(image_start, image_size, ph, proc) != NOS_OK)
        return -1;
    }
  }

  return NOS_OK;
}

static int
load_elf_program(struct process *proc, uintptr_t image_start, size_t image_size)
{
  int rc;
  struct elf_header *header = (struct elf_header *)image_start;
  uint32_t phentsize = header->e_phentsize;
  uint32_t phnum = header->e_phnum;

  if (header->e_magic != ELF_MAGIC)
    return -1;

  if (header->e_phoff == 0 || header->e_phoff >= image_size)
    return -1;

  rc = load_from_prog_headers(
      image_start, image_size,
      (struct elf_prog_header *)((uint8_t *)image_start + header->e_phoff),
      phentsize, phnum, proc);
  if (rc != NOS_OK)
    return -1;

  proc->entry = (void *)(header->e_entry);
  return NOS_OK;

#if 0
  size_t aligned_size = ALIGN_UP(image_size, PAGE_SIZE);
  uintptr_t vaddr_start = (uintptr_t)BINARY_ENTRY_VADDR;
  uintptr_t vaddr = vaddr_start;
  uintptr_t end_vaddr = vaddr + aligned_size;

  for (; vaddr < end_vaddr; vaddr += PAGE_SIZE) {
    phys_addr_t paddr = pmm_alloc_block();
    vmm_map_page(process->pgdir, vaddr, paddr, VMM_WRITABLE | VMM_USER);
  }

  bzero((void *)vaddr_start, aligned_size);
  memcpy((void *)vaddr_start, (void *)image_start, image_size);

  process->entry = (void *)vaddr_start;
#endif
}

int
loader_load_elf(struct process *process, uintptr_t image_start,
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

  // 加载 ELF 可执行程序到进程的地址空间
  if (load_elf_program(process, image_start, image_size) != NOS_OK) {
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

bool
loader_is_elf(uintptr_t image_start, size_t image_size)
{
  if (image_size < sizeof(struct elf_header))
    return false;

  struct elf_header *elf_header = (struct elf_header *)image_start;
  if (elf_header->e_magic != ELF_MAGIC)
    return false;

  return true;
}
