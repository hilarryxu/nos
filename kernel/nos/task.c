#include <stdint.h>

#include <nos/nos.h>
#include <nos/trap.h>

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

static uint8_t stack_a[4096];
static uint8_t stack_b[4096];

struct trap_frame *
task_init(uint8_t *stack, void *entry)
{
  struct trap_frame task_state = {
      .eax = 0,
      .ebx = 0,
      .ecx = 0,
      .edx = 0,
      .esi = 0,
      .edi = 0,

      .cs = 0x08,
      .eip = (uint32_t)entry,
      .eflags = 0x200,
  };

  struct trap_frame *tf = (void *)(stack + 4096 - sizeof(task_state));
  *tf = task_state;

  return tf;
}

static int current_task = -1;
static int num_tasks = 2;
static struct trap_frame *task_states[2];

void
task_setup()
{
  task_states[0] = task_init(stack_a, task_a);
  task_states[1] = task_init(stack_b, task_b);
}

struct trap_frame *
schedule(struct trap_frame *tf)
{
  if (current_task >= 0) {
    task_states[current_task] = tf;
  }

  current_task++;
  current_task %= num_tasks;

  tf = task_states[current_task];

  return tf;
}
