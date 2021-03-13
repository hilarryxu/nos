#ifndef _NOS_SCHED_SCHED_H
#define _NOS_SCHED_SCHED_H

#include <nos/proc/process.h>

// 初始化进程调度子系统
void sched_steup();

// 执行进程调度
void schedule();

void sched_add_process(struct process *);
void sched_remove_process(struct process *);

#endif  // !_NOS_SCHED_SCHED_H
