#ifndef _NOS_PROC_PROCESS_H
#define _NOS_PROC_PROCESS_H

#include <stddef.h>
#include <sys/types.h>

#include <nos/mm/vmm.h>
#include "nc_sys_queue.h"

#define INVALID_PID ((pid_t)-1)

// call switch_to 导致被调用者需要保存这些寄存器（x86 函数调用规范约定的）,
// 最上面的 eip 就是调用 switch_to 返回后的下一条指令地址。
//
// Note: 除了第一次运行进程需要自己修改 eip 换成自己的处理函数
// 最终执行 iret 切换到用户态运行外（即在用户进程自己的内核栈上
// 构造中断栈帧以及上下文）
// 其他情况下多半就是时钟中断那里自动压栈切换到其他进程再切换回来
// 继续运行直到中断返回又回到了用户态，无需手工干涉了。
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
  TAILQ_ENTRY(process) process_link;
  char name[16];
};

void process_setup();

struct process *process_alloc();
void process_destory(struct process *process);

int process_exec_binary(const char *binary, size_t size,
                        struct process **p_process);
void process_exit(struct process *process, int exit_code);

#endif  // _NOS_PROC_PROCESS_H
