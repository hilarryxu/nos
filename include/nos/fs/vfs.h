#ifndef _NOS_FS_VFS_H
#define _NOS_FS_VFS_H

#include <stddef.h>
#include <stdint.h>

struct inode;
struct file_operations;
struct mount;

struct vfs_node {
  char *name;
  const struct mount *mount;
  const struct file_operations *op;
  struct inode *inode;

  uint32_t pos;
  int refs_nr;
  int flags;
};

struct file_operations {
  int (*open)(struct vfs_node *, int);
  int (*close)(struct vfs_node *);
  int (*read)(struct vfs_node *, char *, size_t);
  int (*write)(struct vfs_node *, const char *, size_t);
};

struct inode {
  uint8_t device;
  uint32_t ino;
  void *priv_data;
};

struct vfs_filesystem {
  const char *name;
  const struct file_operations *op;
  void (*initialize)();
};

struct mount {
  uint8_t device;
  const char *root;
  const struct vfs_filesystem *fs;
};

struct dirent {
  char name[256];
  uint32_t ino;
};

struct vfs_stat {
  uint32_t size;
  uint32_t mode;
};

void vfs_setup();
struct vfs_node *vfs_alloc_vnode();
void vfs_free_vnode(struct vfs_node *vnode);

int vfs_open(struct vfs_node *vnode, const char *path, int flags);
int vfs_read(struct vfs_node *vnode, char *buffer, size_t nbytes);
int vfs_write(struct vfs_node *vnode, const char *data, size_t nbytes);
int vfs_close(struct vfs_node *vnode);

struct dirent *vfs_readdir(struct vfs_node *vnode, uint32_t index);
struct vfs_node *vfs_finddir(struct vfs_node *vnode, const char *name);
int vfs_mkdir(struct vfs_node *vnode, const char *name);
int vfs_stat(struct vfs_node *vnode, struct vfs_stat *st);

struct vfs_node *vfs_mount(const char *path, struct vfs_node *root);
struct vfs_node *vfs_unmount(const char *path);
struct vfs_node *vfs_namei(const char *path);

#endif  // _NOS_FS_VFS_H
