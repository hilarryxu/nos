#include <stdint.h>

#include <nos/nos.h>
#include <nos/trap.h>
#include <nos/gdt.h>
#include <nos/mm/pmm.h>
#include <nos/multiboot.h>
#include <inc/elf.h>

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

static inline void *
memset(void *buf, uint8_t c, size_t n)
{
  uint8_t *p = buf;

  while (n--) {
    *p++ = c;
  }

  return buf;
}

static inline void *
memcpy(void *dst, const void *src, size_t n)
{
  uint8_t *d = dst;
  const uint8_t *s = src;

  while (n--) {
    *d++ = *s++;
  }

  return dst;
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
