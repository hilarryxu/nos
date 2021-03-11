#ifndef _NOS_PROC_SCHEDULER_H
#define _NOS_PROC_SCHEDULER_H

#include <nos/proc/process.h>

#define EFLAGS_IF (1 << 9)

extern struct process *current_process;

void sched_steup();

void sched_add_process(struct process *);

void sched_remove_process(struct process *);

void sched();

void scheduler();

void yield();

#endif  // _NOS_PROC_SCHEDULER_H
