#include <stdint.h>
#include <string.h>

#include <nos/nos.h>
#include <nos/trap.h>
#include <nos/gdt.h>
#include <nos/mm/pmm.h>
#include <nos/multiboot.h>
#include <inc/elf.h>
#include <nos/mm/kheap.h>
#include <nos/mm/vaddr_space.h>
#include <nos/proc/process.h>
#include <nos/proc/scheduler.h>

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

void
task_init(void *entry)
{
  struct process *ktask = process_alloc();

  ktask->kernel_stack = kmalloc(PAGE_SIZE);
  ktask->kernel_stack += PAGE_SIZE;

  ktask->page_dir = vaddr_space_create(&ktask->page_dir_phys);
  if (!ktask->page_dir)
    log_panic("vaddr_space_create failed");

  ktask->state = PROCESS_STATE_RUNNABLE;
  ktask->entry = entry;

  sched_add_process(ktask);
}

void
load_elf(void *image)
{
  struct Elf *elfhdr = (struct Elf *)image;
  struct Proghdr *ph, *eph;

  if (elfhdr->e_magic != ELF_MAGIC) {
    printk("Not a elf app\n");
    return;
  }

  ph = (struct Proghdr *)((uint8_t *)image + elfhdr->e_phoff);
  eph = ph + elfhdr->e_phnum;
  for (; ph < eph; ph++) {
    if (ph->p_type != ELF_PROG_LOAD) {
      continue;
    }
    memset((void *)ph->p_pa, 0, ph->p_memsz);
    memcpy((void *)ph->p_pa, ((uint8_t *)image) + ph->p_offset, ph->p_filesz);
  }

  task_init((void *)elfhdr->e_entry);
}

void
task_setup(struct multiboot_info *mb_info)
{
  if (mb_info && mb_info->mods_count > 0) {
    multiboot_module_t *modules = (multiboot_module_t *)mb_info->mods_addr;
    for (size_t i = 0; i < mb_info->mods_count; i++) {
      load_elf((void *)modules[i].mod_start);
    }
  } else {
    task_init(task_a);
    task_init(task_b);
    task_init(task_c);
    task_init(task_d);
  }
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
