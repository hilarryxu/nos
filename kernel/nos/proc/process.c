#include <nos/proc/process.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/mm/paging.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>
#include <nos/mm/kheap.h>
#include <nos/mm/vaddr_space.h>

#define PROCESS_KERNEL_STACK (KERNEL_BASE - (16 * PAGE_SIZE))
#define PROCESS_USER_STACK (KERNEL_BASE - (1024 * PAGE_SIZE))

static pid_t next_pid = 0;

static pid_t
alloc_pid()
{
  return ++next_pid;
}

//---------------------------------------------------------------------
// 分配一个 process 结构体
//---------------------------------------------------------------------
struct process *
process_alloc()
{
  struct process *proc = kmalloc(sizeof(*proc));
  if (!proc)
    return NULL;

  bzero(proc, sizeof(*proc));
  proc->pid = alloc_pid();

  if (proc->pid == -1) {
    process_destory(proc);
    return NULL;
  }

  return proc;
}

//---------------------------------------------------------------------
// 销毁进程
//---------------------------------------------------------------------
void
process_destory(struct process *proc)
{
  // TODO: free proc
  kfree(proc);
}

// 为进程分配用户堆栈和内核堆栈
static int
alloc_proc_stacks(struct process *proc)
{
  phys_addr_t stack = pmm_alloc_block();
  if (!stack)
    return -1;

  if (vmm_map_page((uintptr_t)(PROCESS_KERNEL_STACK - PAGE_SIZE), stack,
                   VMM_WRITABLE) < 0) {
    pmm_free_block(stack);
    return -1;
  }

  stack = pmm_alloc_block();
  // FIXME: unmap prev stack
  if (!stack)
    return -1;

  if (vmm_map_page((uintptr_t)(PROCESS_USER_STACK - PAGE_SIZE), stack,
                   VMM_WRITABLE | VMM_USER) < 0) {
    pmm_free_block(stack);
    return -1;
  }

  proc->kernel_stack = (char *)PROCESS_KERNEL_STACK;
  proc->user_stack = (char *)PROCESS_USER_STACK;

  return NOS_OK;
}

#if 0
static int
init_from_elf(struct process *proc, const char *elf, size_t size)
{
  // TODO: need page_dir's phys_addr
  proc->page_dir = vaddr_space_create(NULL);
  if (!proc->page_dir)
    return -1;

  if (load_elf_program(proc, elf, size) != NOS_OK)
    return -1;

  if (alloc_proc_stacks(proc) != NOS_OK)
    return -1;

  return NOS_OK;
}
#endif

static int
load_binary_program(struct process *proc, const char *binary, size_t size)
{
  // binary must 4KB aligned
  size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
  uintptr_t vaddr = (uintptr_t)binary;
  uintptr_t end_vaddr = vaddr + aligned_size;

  for (; vaddr < end_vaddr; vaddr += PAGE_SIZE) {
    phys_addr_t paddr = pmm_alloc_block();
    vmm_map_page(vaddr, paddr, VMM_WRITABLE | VMM_USER);
  }

  proc->entry = (void *)0x200000;

  return NOS_OK;
}

static int
init_from_binary(struct process *proc, const char *binary, size_t size)
{
  proc->page_dir = vaddr_space_create(&proc->page_dir_phys);
  if (!proc->page_dir)
    return -1;

  vmm_switch_pgdir(proc->page_dir_phys);

  if (load_binary_program(proc, binary, size) != NOS_OK)
    return -1;

  if (alloc_proc_stacks(proc) != NOS_OK)
    return -1;

  return NOS_OK;
}

int
process_exec(const char *binary, size_t size, struct process **p_proc)
{
  struct process *proc = process_alloc();
  if (!proc)
    return -1;

  if (init_from_binary(proc, binary, size) != NOS_OK) {
    process_destory(proc);
    return -1;
  }

  proc->state = PROCESS_STATE_RUNNING;
  if (p_proc) {
    *p_proc = proc;
  }

  // TODO: add to scheduler
  return NOS_OK;
}

void
process_exit(struct process *proc, int exit_code)
{
  proc->exit_code = exit_code;
  proc->state = PROCESS_STATE_DEAD;
  // TODO: sched
  // sched()
}

void
process_setup()
{
}
