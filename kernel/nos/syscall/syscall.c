#include <nos/syscall/syscall.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/current.h>
#include <nos/trap.h>
#include <nos/proc/process.h>
#include <nos/fs/vfs.h>
#include <nos/fs/osfile.h>
#include <nos/mm/kheap.h>

static long
sys_demo(char ch)
{
  printk("%c", ch);
  return NOS_OK;
}

static long
sys_open(const char *pathname, int flags)
{
  int rc;
  int fd = -1;
  struct vfs *vfs;
  struct file *file;
  struct osfile *osfile;
  struct process *proc = current;

  for (int i = 0; i < NR_PROCESS_FILES; i++) {
    if (proc->files[i] == NULL) {
      fd = i;
      break;
    }
  }

  if (fd < 0)
    return -1;

  vfs = vfs_find(pathname);
  if (vfs == NULL)
    return -1;

  rc = vfs_open_malloc(vfs, pathname, &file, flags, NULL);
  if (rc != NOS_OK)
    return -1;

  osfile = (struct osfile *)kmalloc(sizeof(*osfile));
  bzero(osfile, sizeof(*osfile));
  osfile->f_file = file;
  osfile->refcount += 1;

  proc->files[fd] = osfile;

  return fd;
}

static long
sys_close(int fd)
{
  struct process *proc = current;
  struct osfile *osfile;

  if (fd < 0 || fd > NR_PROCESS_FILES)
    return -1;

  osfile = proc->files[fd];
  if (osfile == NULL)
    return -1;

  osfile->refcount -= 1;
  if (osfile->refcount == 0) {
    vfs_close_free(osfile->f_file);
    kfree(osfile);
  }

  proc->files[fd] = NULL;

  return NOS_OK;
}

static long
sys_read(int fd, void *buf, size_t len)
{
  struct process *proc = current;
  struct osfile *osfile;

  if (fd < 0 || fd > NR_PROCESS_FILES)
    return -1;

  osfile = proc->files[fd];
  if (osfile == NULL)
    return -1;

  int nread = vfs_read(osfile->f_file, buf, len, osfile->offset);
  if (nread > 0) {
    osfile->offset += nread;
  }

  return nread;
}

static long
sys_write(int fd, const void *buf, size_t len)
{
  struct process *proc = current;
  struct osfile *osfile;

  if (fd < 0 || fd > NR_PROCESS_FILES)
    return -1;

  osfile = proc->files[fd];
  if (osfile == NULL)
    return -1;

  int nwrite = vfs_write(osfile->f_file, buf, len, osfile->offset);
  if (nwrite > 0) {
    osfile->offset += nwrite;
  }

  return nwrite;
}

static long
sys_getpid()
{
  return current->pid;
}

static void *syscall_table[] = {
    [0] = sys_demo,           [__NR_read] = sys_read,
    [__NR_write] = sys_write, [__NR_open] = sys_open,
    [__NR_close] = sys_close, [__NR_getpid] = sys_getpid,
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
