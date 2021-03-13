#include <nos/sched/sched.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/arch.h>
#include <nos/gdt.h>
#include <nos/trap.h>
#include "nc_sys_queue.h"

TAILQ_HEAD(process_list, process);

// 当前进程
struct process *current_process = NULL;

struct process *idle_process = NULL;

// 进程队列
static struct process_list process_list;

//---------------------------------------------------------------------
// 进程调度
//---------------------------------------------------------------------
void
schedule()
{
  bool intr_flag;
  struct process *p = NULL;
  int loop_round;

  local_intr_save(intr_flag);
  {
    ASSERT(current_process != NULL);

    // 循环遍历一边进程队列，找到下一个可运行进程
    loop_round = 0;
    p = current_process == idle_process
            ? TAILQ_NEXT(current_process, process_link)
            : TAILQ_FIRST(&process_list);
    while (loop_round < 2) {
      if (p == NULL) {
        if (loop_round == 0) {
          p = TAILQ_FIRST(&process_list);
        }
        loop_round++;
        continue;
      }
      // 循环遍历一轮没找到其他可运行进程
      if (current_process && p == current_process) {
        ASSERT(loop_round <= 1);
        p = NULL;
        break;
      }

      // 找到一个可运行进程了
      if (p->state == PROCESS_STATE_RUNNABLE) {
        break;
      }

      p = TAILQ_NEXT(p, process_link);
    }

    // 找不到就运行 idle
    if (p == NULL || p->state != PROCESS_STATE_RUNNABLE) {
      p = idle_process;
    }

    // 还是当前进程的话不用调度
    if (p != current_process) {
      // 调度运行选出的进程
      process_run(p);
    }
  }
  local_intr_restore(intr_flag);
}

void
sched_add_process(struct process *process)
{
  TAILQ_INSERT_TAIL(&process_list, process, process_link);
}

void
scheduler_remove_process(struct process *process)
{
  TAILQ_REMOVE(&process_list, process, process_link);
}

void
sched_steup()
{
  TAILQ_INIT(&process_list);

  // FIXME: ltr, tss
}
