#include <nos/proc/scheduler.h>

#include <string.h>

#include "nc_sys_queue.h"

#include <nos/nos.h>
#include <nos/arch.h>
#include <nos/gdt.h>
#include <nos/trap.h>

#define EFLAGS_IF (1 << 9)

extern void switch_to(struct kstack_context **old, struct kstack_context *new);
extern void trapret();

TAILQ_HEAD(processes_hdr, process);

// 调度器上下文
static struct kstack_context *scheduler_context;

// 当前进程
static struct process *current_process;

static struct processes_hdr processes_hdr;

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
init_process_context(struct process *process)
{
  char *sp = NULL;  // 栈帧

  // TODO: kernel task

  init_trap_frame(process);
  sp = (char *)process->tf;

  sp -= sizeof(*process->context);
  process->context = (struct kstack_context *)sp;
  bzero(process->context, sizeof(*process->context));
  process->context->eip = (uint32_t)trapret;
}

// 调度器（被 kernel_main 调用）
void
scheduler()
{
  struct process *p;
  current_process = NULL;

  for (;;) {
    interrupt_enable();

    // lock
    // 循环遍历一边进程队列，找到下一个可运行进程
    int iter_loop = 0;
    p = current_process ? TAILQ_NEXT(current_process, processes)
                        : TAILQ_FIRST(&processes_hdr);
    if (!p) {
      p = TAILQ_FIRST(&processes_hdr);
    }
    while (iter_loop < 2) {
      if (p == NULL) {
        iter_loop++;
        continue;
      }
      if (current_process && p == current_process) {
        ASSERT(iter_loop == 1);
        break;
      }

      if (p->state == PROCESS_STATE_RUNNABLE) {
        // 找到一个可运行进程了
        break;
      }

      p = TAILQ_NEXT(p, processes);
    }

    // TODO: 找不到就运行 idle 进程
    ASSERT(p != NULL);
    current_process = p;
    tss_set_kstack((uint32_t)p->kernel_stack);
    vmm_switch_pgdir(p->page_dir_phys);
    if (!p->context) {
      init_process_context(p);
    }
    p->state = PROCESS_STATE_RUNNING;

    switch_to(&scheduler_context, p->context);

    current_process = NULL;
    // unlock
  }
}

void
scheduler_add_process(struct process *process)
{
  TAILQ_INSERT_TAIL(&processes_hdr, process, processes);
}

void
scheduler_remove_process(struct process *process)
{
  TAILQ_REMOVE(&processes_hdr, process, processes);
}

void
scheduler_steup()
{
  TAILQ_INIT(&processes_hdr);
}
