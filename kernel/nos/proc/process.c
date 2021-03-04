#include <string.h>

#include <nos/nos.h>
#include <nos/mm/paging.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>
#include <nos/mm/kheap.h>

typedef short pid_t;

#define PROCESS_KERNEL_STACK (KERNEL_VIRTUAL_START - 16 * PAGE_SIZE)
#define PROCESS_USER_STACK (KERNEL_VIRTUAL_START - 1024 * PAGE_SIZE)

struct kstack_context {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebx;
  uint32_t ebp;
  uint32_t eip;
};

enum process_state {
  PROCESS_STATE_RUNNABLE = 1,
  PROCESS_STATE_RUNNING,
  PROCESS_STATE_SLEEPING,
  PROCESS_STATE_DEAD,
};

struct process {
  pid_t pid;
  enum process_state state;
  struct page_directory *page_dir;
  struct trap_frame *tf;
  struct kstack_context *context;
  char *kstack;
  char *user_stack;
  void *entry;
  int exit_status;
  struct process *next;
  char name[16];
};

static pid_t next_pid = 0;

void process_destory(struct process *proc);

static pid_t
alloc_pid()
{
  return ++next_pid;
}

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

void
process_destory(struct process *proc)
{
  // TODO: free proc
  kfree(proc);
}

static int
alloc_proc_stacks(struct process *proc)
{
  phys_addr_t stack = pmm_alloc_block();
  if (!stack)
    return -1;
  if (vmm_map_page(proc->page_dir,
                   (uintptr_t)(PROCESS_KERNEL_STACK - PAGE_SIZE), stack,
                   VMM_WRITABLE) < 0) {
    pmm_free_block(stack);
    return -1;
  }

  stack = pmm_alloc_block();
  // FIXME: unmap prev stack
  if (!stack)
    return -1;
  if (vmm_map_page(proc->page_dir, (uintptr_t)(PROCESS_USER_STACK - PAGE_SIZE),
                   stack, VMM_WRITABLE | VMM_USER) < 0) {
    pmm_free_block(stack);
    return -1;
  }

  proc->kstack = (char *)PROCESS_KERNEL_STACK;
  proc->user_stack = (char *)PROCESS_USER_STACK;
  return NOS_OK;
}

static int
load_elf_program(struct process *proc, const char *elf, size_t size)
{
  (void)proc;
  (void)elf;
  (void)size;

  return -1;
}

static int
init_from_elf(struct process *proc, const char *elf, size_t size)
{
  // TODO: need page_dir's phys_addr
  proc->page_dir = vmm_alloc_vaddr_space();
  if (!proc->page_dir)
    return -1;

  if (load_elf_program(proc, elf, size) != NOS_OK)
    return -1;

  if (alloc_proc_stacks(proc) != NOS_OK)
    return -1;

  paging_copy_kernel_space(proc->page_dir);
  return 0;
}

int
process_exec(const char *elf, size_t size)
{
  struct process *proc = process_alloc();
  if (!proc)
    return -1;

  if (init_from_elf(proc, elf, size) != NOS_OK) {
    process_destory(proc);
    return -1;
  }

  proc->state = PROCESS_STATE_RUNNING;
  // TODO: add to scheduler
  return NOS_OK;
}

void
process_exite(struct process *proc, int exit_status)
{
  proc->exit_status = exit_status;
  proc->state = PROCESS_STATE_DEAD;
  // TODO: sched
  // sched()
}
