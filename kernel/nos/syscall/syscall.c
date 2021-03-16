#include <nos/syscall/syscall.h>

#include <nos/nos.h>
#include <nos/current.h>
#include <nos/trap.h>
#include <nos/proc/process.h>

static long
sys_demo(char ch)
{
  printk("%c", ch);
  return NOS_OK;
}

static long
sys_getpid()
{
  return current->pid;
}

static void *syscall_table[] = {
    [0] = sys_demo,
    [__NR_getpid] = sys_getpid,
};

#define NR_SYSCALLS NELEMS(syscall_table)

void
syscall_dispatch(struct trap_frame *tf)
{
  uint32_t ret;
  uint32_t syscall_num = tf->eax;

  if (syscall_num < NR_SYSCALLS) {
    void *handler = syscall_table[syscall_num];

    asm volatile(" \
      pushl %1; \
      pushl %2; \
      pushl %3; \
      pushl %4; \
      pushl %5; \
      call *%6; \
      popl %%ebx; \
      popl %%ebx; \
      popl %%ebx; \
      popl %%ebx; \
      popl %%ebx; \
    "
                 : "=a"(ret)
                 : "r"(tf->edi), "r"(tf->esi), "r"(tf->edx), "r"(tf->ecx),
                   "r"(tf->ebx), "r"(handler));
    tf->eax = ret;
    return;
  }

  log_panic("undefined syscall %d, pid = %d, name = %s", syscall_num,
            current_process->pid, current_process->name);
}
