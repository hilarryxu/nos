#include <string.h>

#include <nos/nos.h>
#include <nos/arch.h>
#include <nos/gdt.h>
#include <nos/trap.h>
#include <nos/proc/process.h>

#define EFLAGS_IF (1 << 9)

void switch_to(struct kstack_context **old, struct kstack_context *new);

// 调度器上下文
static struct kstack_context *scheduler_context;

// 当前进程
static struct process *current_process;

// 切换到调度器去执行进程调度
void
sched()
{
  // FIXME: chekck process state, eflags
  // 保存当前进程上下文，切换到调度器去执行
  switch_to(&current_process->context, scheduler_context);
}

// 主动让出 CPU 给其他进程使用
void
yield_cpu()
{
  current_process->state = PROCESS_STATE_RUNNABLE;
  sched();
}

static void
init_trap_frame(struct process *process)
{
  char *kstack = (char *)process->kernel_stack;
  process->tf = (struct trap_frame *)(kstack - sizeof(*process->tf));
  bzero(process->tf, sizeof(*process->tf));

  process->tf->cs = USER_CODE_SELECTOR;

  // process->tf->ds = USER_DATA_SELECTOR;
  // process->tf->gs = USER_DATA_SELECTOR;
  // process->tf->fs = USER_DATA_SELECTOR;
  // process->tf->es = USER_DATA_SELECTOR;
  process->tf->ss = USER_DATA_SELECTOR;

  process->tf->esp = (uint32_t)process->user_stack;
  process->tf->eflags = EFLAGS_IF;
  process->tf->eip = (uint32_t)process->entry;
  process->tf->error_code = 0;
}

static void
ret_process_entry()
{
  // ret_to_userspace(current_process->tf);
}

static void
init_process_context(struct process *process)
{
  char *sp = NULL;  // 栈帧

  // TODO: kernel task

  init_trap_frame(process);
  sp = (char *)process->tf;

  sp -= sizeof(*process->context);
  process->context = (struct kstack_context *)sp;
  bzero(process->context, sizeof(*process->context));
  process->context->eip = (uint32_t)ret_process_entry;
}

// 调度器（被 kernel_main 调用）
void
scheduler()
{
  struct process *p = NULL;
  current_process = NULL;

  for (;;) {
    interrupt_enable();

    // lock
    // p <-
    current_process = p;
    // tss
    // cr3
    // first init process
    if (!p->context) {
      init_process_context(p);
    }
    p->state = PROCESS_STATE_RUNNING;

    switch_to(&scheduler_context, p->context);

    current_process = NULL;
    // unlock
  }
}
