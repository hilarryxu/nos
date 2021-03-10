#include <nos/fs/jamfs.h>

#include <nos/nos.h>

#define PATH_MAX 512

struct jamfs_file {
  struct file base;
};

static int
jamfs_close(struct file *file)
{
  UNUSED(file);

  return -1;
}

static int
jamfs_read(struct file *file, void *buf, size_t nbytes)
{
  UNUSED(file);
  UNUSED(buf);
  UNUSED(nbytes);

  return -1;
}

static int
jamfs_write(struct file *file, const void *buf, size_t nbytes)
{
  UNUSED(file);
  UNUSED(buf);
  UNUSED(nbytes);

  return -1;
}

static struct file_meths jamfs_file_meths = {.version = 1,
                                             .close = jamfs_close,
                                             .read = jamfs_read,
                                             .write = jamfs_write};

static int
jamfs_open(struct vfs *vfs, const char *path, struct file *file, int flags,
           int *p_out_flags)
{
  UNUSED(vfs);
  UNUSED(path);

  struct jamfs_file *p = (struct jamfs_file *)file;

  if (p_out_flags)
    *p_out_flags = flags;

  p->base.meths = &jamfs_file_meths;

  return NOS_OK;
}

struct vfs jamfs = {.version = 1,
                    .file_struct_sz = sizeof(struct jamfs_file),
                    .max_pathname = PATH_MAX,
                    .next = NULL,
                    .name = "jamfs",
                    .opaque = NULL,
                    .open = jamfs_open};
