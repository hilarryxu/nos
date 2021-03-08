#ifndef _NOS_PROC_SCHEDULER_H
#define _NOS_PROC_SCHEDULER_H

#include <nos/proc/process.h>

void scheduler_steup();

void scheduler_add_process(struct process *);

void scheduler_remove_process(struct process *);

void sched();

void scheduler();

void yield_cpu();

#endif  // _NOS_PROC_SCHEDULER_H
