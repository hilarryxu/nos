#include <nos/fs/vfs.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/fs/jamfs.h>
#include <nos/mm/kheap.h>

#define VFS_ROOT_DIR '/'

struct mount {
  const char *mount_point;
  struct vfs *vfs;
  struct mount *next;
};

extern struct vfs jamfs;

static struct mount *vfs_mount_list = NULL;

void
vfs_setup()
{
  vfs_mount("/", &jamfs);
}

int
vfs_open(struct vfs *vfs, const char *path, struct file *file, int flags,
         int *p_out_flags)
{
  int rc;

  rc = vfs->open(vfs, path, file, flags, p_out_flags);
  ASSERT(rc == NOS_OK || file->meths == NULL);

  return rc;
}

void
vfs_close(struct file *file)
{
  if (file->meths) {
    file->meths->close(file);
    file->meths = NULL;
  }
}

int
vfs_read(struct file *file, void *buf, size_t nbytes, off_t offset)
{
  return file->meths->read(file, buf, nbytes, offset);
}

int
vfs_write(struct file *file, const void *buf, size_t nbytes, off_t offset)
{
  return file->meths->write(file, buf, nbytes, offset);
}

int
vfs_mount(const char *path, struct vfs *vfs)
{
  struct mount *mount = kmalloc(sizeof(*mount));
  if (!mount)
    return -1;

  mount->mount_point = path;
  mount->vfs = vfs;

  if (vfs_mount_list == NULL) {
    mount->next = vfs_mount_list;
    vfs_mount_list = mount;
  } else {
    mount->next = vfs_mount_list->next;
    vfs_mount_list->next = mount;
  }

  ASSERT(vfs_mount_list);
  return NOS_OK;
}

int
vfs_unmount(const char *path)
{
  // TODO:
  UNUSED(path);

  return -1;
}

struct vfs *
vfs_find(const char *path)
{
  if (*path != VFS_ROOT_DIR)
    return NULL;

  ASSERT(vfs_mount_list && vfs_mount_list->vfs);
  return vfs_mount_list->vfs;
}

int
vfs_open_malloc(struct vfs *vfs, const char *path, struct file **p_file,
                int flags, int *p_out_flags)
{
  int rc;
  struct file *file = (struct file *)kmalloc(vfs->file_struct_sz);
  if (file) {
    rc = vfs_open(vfs, path, file, flags, p_out_flags);
    if (rc != NOS_OK) {
      kfree(file);
    } else {
      *p_file = file;
    }
  } else {
    rc = -1;
  }

  return rc;
}

void
vfs_close_free(struct file *file)
{
  ASSERT(file);

  vfs_close(file);
  kfree(file);
}
