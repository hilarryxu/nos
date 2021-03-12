#include <nos/proc/scheduler.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/arch.h>
#include <nos/gdt.h>
#include <nos/trap.h>
#include "nc_sys_queue.h"

extern void switch_to(struct kstack_context **old, struct kstack_context *new);
extern void trapret();

TAILQ_HEAD(run_queue, process);

// 调度器上下文
static struct kstack_context *sched_context;

// 当前进程
struct process *current_process = NULL;

// 进程队列
static struct run_queue run_queue;

// 切换到调度器去执行进程调度
void
sched()
{
  // FIXME: chekck process state, eflags
  // 保存当前进程上下文，切换到调度器去执行
  switch_to(&current_process->context, sched_context);
}

// 主动让出 CPU 给其他进程使用
void
yield()
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

// 调度器（被 kernel_main 调用）
void
scheduler()
{
  struct process *p;
  current_process = NULL;

  for (;;) {
    intr_enable();

    // lock
    // 循环遍历一边进程队列，找到下一个可运行进程
    int iter_loop = 0;
    p = current_process ? TAILQ_NEXT(current_process, process_link)
                        : TAILQ_FIRST(&run_queue);
    while (iter_loop < 2) {
      if (p == NULL) {
        if (iter_loop == 0) {
          p = TAILQ_FIRST(&run_queue);
        }
        iter_loop++;
        continue;
      }
      if (current_process && p == current_process) {
        ASSERT(iter_loop <= 1);
        break;
      }

      // FIXME: handle dead process

      if (p->state >= PROCESS_STATE_RUNNABLE) {
        // 找到一个可运行进程了
        break;
      }

      p = TAILQ_NEXT(p, process_link);
    }

    // interrupt_disable();
    // loga("current_process = %x, p = %x", current_process, p);

    // TODO: 找不到就运行 idle 进程
    ASSERT(p != NULL);
    current_process = p;

    // 更新 TSS
    tss_set_kstack((uint32_t)p->kernel_stack);

    // 切换地址空间
    vmm_switch_pgdir(p->page_dir_phys);

    // 首次运行初始化入口内核栈
    if (!p->context) {
      init_process_context(p);
    }

    // 设置进程状态为运行中
    p->state = PROCESS_STATE_RUNNING;

    // 切换上下文，运行进程
    switch_to(&sched_context, p->context);

    // TODO: 中断重入问题？
    // unlock
  }
}

void
sched_add_process(struct process *process)
{
  TAILQ_INSERT_TAIL(&run_queue, process, process_link);
}

void
scheduler_remove_process(struct process *process)
{
  TAILQ_REMOVE(&run_queue, process, process_link);
}

void
sched_steup()
{
  TAILQ_INIT(&run_queue);

  // FIXME: ltr, tss
}
