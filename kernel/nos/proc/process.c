#include <nos/proc/process.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/sync/sync.h>
#include <nos/gdt.h>
#include <nos/mm/paging.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>
#include <nos/mm/kheap.h>
#include <nos/mm/vaddr_space.h>
#include <nos/sched/sched.h>
#include <nos/bin_loader/loader.h>

#define PROCESS_USER_STACK (KERNEL_BASE - (1024 * PAGE_SIZE))

extern char boot_kstack[];

extern void switch_to(struct kstack_context **old, struct kstack_context *new);
extern void trapret();

static pid_t next_pid = 0;

// 当前进程
struct process *current_process = NULL;

// idle 内核线程
struct process *idle_process = NULL;

// 分配进程 ID
static pid_t
alloc_pid()
{
  return next_pid++;
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

  process->state = PROCESS_STATE_UNINIT;
  process->need_resched = false;

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
  process->tf->eflags = FL_IF;
  process->tf->eip = (uint32_t)process->entry;
  process->tf->error_code = 0;
}

static void
init_process_context(struct process *process)
{
  char *sp = NULL;  // 栈帧
  bool is_ktask = false;

  if (is_ktask) {
    sp = (char *)process->kernel_stack;
  } else {
    init_trap_frame(process);
    sp = (char *)process->tf;
  }

  sp -= sizeof(*process->context);
  process->context = (struct kstack_context *)sp;
  bzero(process->context, sizeof(*process->context));
  if (is_ktask) {
    process->context->eip = (uint32_t)process->entry;
  } else {
    process->context->eip = (uint32_t)trapret;
  }
}

// 为进程分配用户堆栈和内核堆栈
int
alloc_process_stacks(struct process *process)
{
  uintptr_t kernel_stack = (uintptr_t)kmalloc_ap(KERNEL_STACK_POW2);

  phys_addr_t stack = pmm_alloc_block();
  if (!stack) {
    return -1;
  }

  if (vmm_map_page((uintptr_t)(PROCESS_USER_STACK - PAGE_SIZE), stack,
                   VMM_WRITABLE | VMM_USER) < 0) {
    pmm_free_block(stack);
    return -1;
  }

  process->kernel_stack = kernel_stack + KERNEL_STACK_SIZE;
  process->user_stack = (uintptr_t)PROCESS_USER_STACK;

  return NOS_OK;
}

//---------------------------------------------------------------------
// 运行二进制可执行程序
//---------------------------------------------------------------------
int
process_exec_image(uintptr_t image_start, size_t image_size,
                   struct process **p_process)
{
  struct process *process = process_alloc();
  if (!process)
    return -1;

  if (loader_load_image(process, image_start, image_size) != NOS_OK) {
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

char *
process_set_name(struct process *process, const char *name)
{
  bzero(process->name, sizeof(process->name));
  return strcpy(process->name, name);
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
      if (next->pid != 0 && !next->context) {
        init_process_context(next);
      }
      // 切换上下文，运行进程
      // loga("switch_to(%d, %d)", prev->pid, next->pid);
      switch_to(&(prev->context), next->context);
    }
    local_intr_restore(intr_flag);
  }
}

//---------------------------------------------------------------------
// 初始化进程子系统
//---------------------------------------------------------------------
void
process_setup()
{
  // 初始化 idle 内核线程
  idle_process = process_alloc();
  ASSERT(idle_process && idle_process->pid == 0);

  idle_process->kernel_stack = (uintptr_t)boot_kstack + KERNEL_STACK_SIZE;
  idle_process->need_resched = true;
  process_set_name(idle_process, "idle");

  idle_process->pgdir = kernel_pgdir;
  idle_process->cr3 = kernel_pgdir_phys;

  current_process = idle_process;
}
