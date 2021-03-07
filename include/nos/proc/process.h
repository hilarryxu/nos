#ifndef _NOS_PROC_PROCESS_H
#define _NOS_PROC_PROCESS_H

#include <stddef.h>

#include <nos/mm/vmm.h>

typedef short pid_t;

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
  phys_addr_t page_dir_phys;
  struct trap_frame *tf;
  struct kstack_context *context;
  char *kernel_stack;
  char *user_stack;
  void *entry;
  int exit_code;
  size_t flags;
  struct process *next;
  char name[16];
};

void process_setup();

int process_exec(const char *binary, size_t size, struct process **p_proc);

void process_destory(struct process *proc);

void process_exit(struct process *proc, int exit_code);

#endif  // _NOS_PROC_PROCESS_H
