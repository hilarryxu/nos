#ifndef _NOS_PROC_PROCESS_H
#define _NOS_PROC_PROCESS_H

#include <stddef.h>
#include <stdbool.h>

#include <nos/types.h>
#include <nos/trap.h>
#include <nos/mm/vmm.h>
#include "nc_sys_queue.h"

#define INVALID_PID ((pid_t)-1)

#define PROCESS_KSTACK_MAGIC 0xDEADABCD

#define MAX_PROCESS_NAME_LEN 15
#define MAX_PROCESS_NUM 1024

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

// 进程状态
enum process_state {
  PROCESS_STATE_UNINIT = 0,  // 未初始化
  PROCESS_STATE_SLEEPING,    // 睡眠
  PROCESS_STATE_RUNNABLE,    // 就绪或正在运行
  PROCESS_STATE_ZOMBIE,      // 快死了，等待父进程回收资源
};

// 进程结构体 PCB
struct process {
  pid_t pid;                     // 进程 ID
  enum process_state state;      // 进程状态
  struct page_directory *pgdir;  // 虚拟地址空间（指向页目录表）
  phys_addr_t cr3;  // 页目录表物理地址（会被加载到 cr3 寄存器中）
  struct trap_frame *tf;              // 中断帧
  struct kstack_context *context;     // 进程切换函数调用保存上下文
  uintptr_t kernel_stack;             // 内核栈
  uintptr_t user_stack;               // 用户栈
  void *entry;                        // 进程执行入口点
  volatile bool need_resched;         // 是否需要重新调度让出 CPU
  int exit_code;                      // 进程退出码（发给父进程）
  uint32_t flags;                     // 标志位
  TAILQ_ENTRY(process) process_link;  // 关联进程列表
  char name[MAX_PROCESS_NAME_LEN + 1];  // 进程名
  uint32_t kstack_magic;
};

extern struct process *current_process, *idle_process;

void process_setup();

struct process *process_alloc();
void process_destory(struct process *process);

char *process_set_name(struct process *process, const char *name);

// 调度运行进程
void process_run(struct process *process);

// 唤醒进程
void process_wakeup(struct process *process);

int process_exec_binary(const char *binary, size_t size,
                        struct process **p_process);
void process_exit(struct process *process, int exit_code);

// idle 内核线程（循环跑，不会返回）
void cpu_idle() __attribute__((noreturn));

#endif  // !_NOS_PROC_PROCESS_H
