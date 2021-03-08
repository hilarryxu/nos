#include <nos/fs/jamfs.h>

#include <nos/nos.h>

static int
jamfs_open(struct vfs_node *vnode, int flags)
{
  UNUSED(vnode);
  UNUSED(flags);

  return -1;
}

static int
jamfs_close(struct vfs_node *vnode)
{
  UNUSED(vnode);

  return -1;
}

static int
jamfs_read(struct vfs_node *vnode, char *buffer, size_t size)
{
  UNUSED(vnode);
  UNUSED(buffer);
  UNUSED(size);

  return -1;
}

static int
jamfs_write(struct vfs_node *vnode, const char *buffer, size_t size)
{
  UNUSED(vnode);
  UNUSED(buffer);
  UNUSED(size);

  return -1;
}

static void
jamfs_initialize()
{
}

static struct file_operations jamfs_ops = {.open = jamfs_open,
                                           .close = jamfs_close,
                                           .read = jamfs_read,
                                           .write = jamfs_write};

const struct vfs_filesystem jamfs = {
    .name = "jamfs", .op = &jamfs_ops, .initialize = jamfs_initialize};
