#ifndef _NOS_FS_VFS_H
#define _NOS_FS_VFS_H

#include <stddef.h>
#include <stdint.h>

#include <nos/types.h>

struct file_meths;

struct file {
  const struct file_meths *meths;
};

struct file_meths {
  int version;
  int (*close)(struct file *);
  int (*read)(struct file *, void *buf, size_t nbytes, off_t offset);
  int (*write)(struct file *, const void *buf, size_t nbytes, off_t offset);
  int (*get_filesize)(struct file *, int64_t *p_size);
};

struct vfs {
  int version;
  int file_struct_sz;
  int max_pathname;
  struct vfs *next;
  const char *name;
  void *opaque;
  int (*open)(struct vfs *, const char *path, struct file *, int flags,
              int *p_out_flags);
};

void vfs_setup();

int vfs_open(struct vfs *, const char *path, struct file *, int flags,
             int *p_out_flags);
void vfs_close(struct file *);
int vfs_read(struct file *, void *buf, size_t nbytes, off_t p_offset);
int vfs_write(struct file *, const void *buf, size_t nbytes, off_t p_offset);

int vfs_get_filesize(struct file *, int64_t *p_size);

int vfs_mount(const char *path, struct vfs *vfs);
int vfs_unmount(const char *path);
struct vfs *vfs_find(const char *path);

int vfs_open_malloc(struct vfs *, const char *path, struct file **, int flags,
                    int *p_out_flags);
void vfs_close_free(struct file *);

#endif  // !_NOS_FS_VFS_H
