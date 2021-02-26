#include <stdint.h>

#include <nos/nos.h>
#include <nos/trap.h>
#include <nos/gdt.h>
#include <nos/mm/pmm.h>

struct task {
  struct trap_frame *tf;
  struct task *next;
};

static struct task *first_task = NULL;
static struct task *current_task = NULL;

static void
task_a()
{
  while (1) {
    printk("A");
  }
}

static void
task_b()
{
  while (1) {
    printk("B");
  }
}

static void
task_c()
{
  while (1) {
    printk("C");
  }
}

static void
task_d()
{
  while (1) {
    printk("D");
  }
}

struct task *
task_init(void *entry)
{
  uint8_t *kstack = (uint8_t *)pmm_alloc();
  uint8_t *user_stack = (uint8_t *)pmm_alloc();

  struct trap_frame task_state = {
      .eax = 0,
      .ebx = 0,
      .ecx = 0,
      .edx = 0,
      .esi = 0,
      .edi = 0,

      .cs = USER_CODE_SELECTOR,
      .eip = (uint32_t)entry,
      .eflags = 0x200,
      .ss = USER_DATA_SELECTOR,
      .esp = (uint32_t)user_stack + 4096,
  };

  struct trap_frame *tf = (void *)(kstack + 4096 - sizeof(task_state));
  *tf = task_state;

  struct task *task = (struct task *)pmm_alloc();
  task->tf = tf;
  task->next = first_task;
  first_task = task;
  return task;
}

void
task_setup()
{
  task_init(task_a);
  task_init(task_b);
  task_init(task_c);
  task_init(task_d);
}

struct trap_frame *
schedule(struct trap_frame *tf)
{
  if (current_task != NULL) {
    current_task->tf = tf;
  }

  if (current_task == NULL) {
    current_task = first_task;
  } else {
    current_task = current_task->next;
    if (current_task == NULL) {
      current_task = first_task;
    }
  }

  tf = current_task->tf;

  return tf;
}
