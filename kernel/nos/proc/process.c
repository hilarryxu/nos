#include <nos/proc/process.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/arch.h>
#include <nos/gdt.h>
#include <nos/mm/paging.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>
#include <nos/mm/kheap.h>
#include <nos/mm/vaddr_space.h>
#include <nos/sched/sched.h>

// 这里让进程的用户栈和内核栈不在同一个页目录项下
#define PROCESS_KERNEL_STACK (KERNEL_BASE - (16 * PAGE_SIZE))
#define PROCESS_USER_STACK (KERNEL_BASE - (1024 * PAGE_SIZE))

extern void switch_to(struct kstack_context **old, struct kstack_context *new);
extern void trapret();

static pid_t next_pid = 0;

// 分配进程 ID
static pid_t
alloc_pid()
{
  return ++next_pid;
}

// 回收进程 ID
static void
free_pid(pid_t pid)
{
  UNUSED(pid);
}

//---------------------------------------------------------------------
// 分配一个 process 结构体
//---------------------------------------------------------------------
struct process *
process_alloc()
{
  struct process *process = kmalloc(sizeof(*process));
  if (!process)
    return NULL;
  bzero(process, sizeof(*process));

  process->pid = alloc_pid();
  if (process->pid == INVALID_PID) {
    process_destory(process);
    return NULL;
  }

  return process;
}

//---------------------------------------------------------------------
// 销毁进程
//---------------------------------------------------------------------
void
process_destory(struct process *process)
{
  // FIXME: free vaddr_space

  if (process->pid != INVALID_PID)
    free_pid(process->pid);

  kfree(process);
}

static void
init_trap_frame(struct process *process)
{
  char *kstack = (char *)process->kernel_stack;
  process->tf = (struct trap_frame *)(kstack - sizeof(*process->tf));
  bzero(process->tf, sizeof(*process->tf));

  process->tf->cs = USER_CODE_SELECTOR;

  process->tf->ds = USER_DATA_SELECTOR;
  process->tf->gs = USER_DATA_SELECTOR;
  process->tf->fs = USER_DATA_SELECTOR;
  process->tf->es = USER_DATA_SELECTOR;
  process->tf->ss = USER_DATA_SELECTOR;

  process->tf->esp = (uint32_t)process->user_stack;
  process->tf->eflags = EFLAGS_IF;
  process->tf->eip = (uint32_t)process->entry;
  process->tf->error_code = 0;
}

static void
init_process_context(struct process *process)
{
  char *sp = NULL;  // 栈帧

  // TODO: handle kernel task

  if (1) {
    sp = (char *)process->kernel_stack;
  } else {
    init_trap_frame(process);
    sp = (char *)process->tf;
  }

  sp -= sizeof(*process->context);
  process->context = (struct kstack_context *)sp;
  bzero(process->context, sizeof(*process->context));
  if (1) {
    process->context->eip = (uint32_t)process->entry;
  } else {
    process->context->eip = (uint32_t)trapret;
  }
}

// 为进程分配用户堆栈和内核堆栈
static int
alloc_process_stacks(struct process *process)
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
  if (!stack)
    return -1;

  if (vmm_map_page((uintptr_t)(PROCESS_USER_STACK - PAGE_SIZE), stack,
                   VMM_WRITABLE | VMM_USER) < 0) {
    pmm_free_block(stack);
    return -1;
  }

  process->kernel_stack = (uintptr_t)PROCESS_KERNEL_STACK;
  process->user_stack = (uintptr_t)PROCESS_USER_STACK;

  return NOS_OK;
}

#if 0
static int
init_from_elf(struct process *proc, const char *elf, size_t size)
{
  // TODO: need page_dir's phys_addr
  process->page_dir = vaddr_space_create(NULL);
  if (!process->page_dir)
    return -1;

  if (load_elf_program(proc, elf, size) != NOS_OK)
    return -1;

  if (alloc_process_stacks(proc) != NOS_OK)
    return -1;

  return NOS_OK;
}
#endif

static int
load_binary_program(struct process *process, const char *binary, size_t size)
{
  // Note: binary ptr addr must 4KB aligned
  size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
  uintptr_t vaddr = (uintptr_t)binary;
  uintptr_t end_vaddr = vaddr + aligned_size;

  for (; vaddr < end_vaddr; vaddr += PAGE_SIZE) {
    phys_addr_t paddr = pmm_alloc_block();
    vmm_map_page(vaddr, paddr, VMM_WRITABLE | VMM_USER);
  }

  process->entry = (void *)0x200000;

  return NOS_OK;
}

static int
init_from_binary(struct process *process, const char *binary, size_t size)
{
  process->pgdir = vaddr_space_create(&process->cr3);
  if (!process->pgdir)
    return -1;

  // TODO: when swith back
  vmm_switch_pgdir(process->cr3);

  // 加载二进制可执行程序到进程的地址空间
  if (load_binary_program(process, binary, size) != NOS_OK)
    return -1;

  if (alloc_process_stacks(process) != NOS_OK)
    return -1;

  return NOS_OK;
}

//---------------------------------------------------------------------
// 运行二进制可执行程序
//---------------------------------------------------------------------
int
process_exec_binary(const char *binary, size_t size, struct process **p_process)
{
  struct process *process = process_alloc();
  if (!process)
    return -1;

  if (init_from_binary(process, binary, size) != NOS_OK) {
    process_destory(process);
    return -1;
  }

  process->state = PROCESS_STATE_RUNNABLE;
  if (p_process) {
    *p_process = process;
  }

  // 添加到进程调度队列中，准备执行
  sched_add_process(process);

  return NOS_OK;
}

//---------------------------------------------------------------------
// 进程退出
//---------------------------------------------------------------------
void
process_exit(struct process *process, int exit_code)
{
  process->exit_code = exit_code;
  process->state = PROCESS_STATE_ZOMBIE;
  // TODO: sched
}

//---------------------------------------------------------------------
// 初始化进程子系统
//---------------------------------------------------------------------
void
process_setup()
{
}

//---------------------------------------------------------------------
// 唤醒进程
//---------------------------------------------------------------------
void
process_wakeup(struct process *process)
{
  ASSERT(process->state != PROCESS_STATE_ZOMBIE);

  bool intr_flag;

  local_intr_save(intr_flag);
  {
    if (process->state != PROCESS_STATE_RUNNABLE) {
      process->state = PROCESS_STATE_RUNNABLE;
    } else {
      log_warn("wakeup runnable process");
    }
  }
  local_intr_restore(intr_flag);
}

//---------------------------------------------------------------------
// idle 内核线程
//---------------------------------------------------------------------
void
cpu_idle()
{
  while (1) {
    ASSERT(current_process != NULL);
    if (current_process->need_resched) {
      // 让出 CPU，调度切换到其他进程
      schedule();
    }
  }
}

//---------------------------------------------------------------------
// 调度运行进程
//---------------------------------------------------------------------
void
process_run(struct process *process)
{
  if (process != current_process) {
    bool intr_flag;
    struct process *prev = current_process;
    struct process *next = process;

    local_intr_save(intr_flag);
    {
      // 设置为当前进程
      current_process = process;
      // 更新 TSS
      tss_set_kstack(next->kernel_stack);
      // 切换地址空间
      vmm_switch_pgdir(next->cr3);
      // 首次运行初始化入口内核栈
      if (!next->context) {
        init_process_context(next);
      }
      // 切换上下文，运行进程
      switch_to(&(prev->context), next->context);
    }
    local_intr_restore(intr_flag);
  }
}
