#include <nos/fs/vfs.h>

#include <string.h>

#include <nos/nos.h>
#include <nos/fs/jamfs.h>
#include <nos/mm/kheap.h>

#define VFS_ROOT_DIR '/'

static struct mount root;

extern const struct vfs_filesystem jamfs;

static const struct mount *
find_mount(const char *path)
{
  if (*path != VFS_ROOT_DIR)
    return NULL;

  return &root;
}

static inline char *
dup_path(const char *path)
{
  char *dup = kmalloc(strlen(path) + 1);
  if (!dup)
    return NULL;
  // return strcpy(dup, path);
  return dup;
}

static inline void
free_path(char **path)
{
  kfree(*path);
  *path = NULL;
}

static inline void
free_inode(struct inode **inode)
{
  kfree(*inode);
  *inode = NULL;
}

static inline struct inode *
alloc_inode()
{
  struct inode *inode = kmalloc(sizeof(*inode));
  if (inode) {
    bzero(inode, sizeof(*inode));
  }
  return inode;
}

void
vfs_setup()
{
  root.device = 1;
  root.root = "/";
  root.fs = &jamfs;

  jamfs.initialize();
}

struct vfs_node *
vfs_alloc_vnode()
{
  struct vfs_node *vnode = kmalloc(sizeof(*vnode));
  if (vnode) {
    bzero(vnode, sizeof(*vnode));
  }
  return vnode;
}

void
vfs_free_vnode(struct vfs_node *vnode)
{
  kfree(vnode);
}

int
vfs_open(struct vfs_node *vnode, const char *path, int flags)
{
  int rc;

  vnode->mount = find_mount(path);
  if (!vnode->mount) {
    log_panic("Find file mount path '%d' failed.", path);
  }

  vnode->path = dup_path(path);
  vnode->inode = alloc_inode();
  vnode->op = vnode->mount->fs->op;
  vnode->flags = flags;

  if (!vnode->path || !vnode->inode) {
    log_panic("Alloc vnode failed.");
  }

  rc = vnode->op->open(vnode, flags);
  if (rc != NOS_OK) {
    free_path(&vnode->path);
    free_inode(&vnode->inode);
  }

  return rc;
}

int
vfs_read(struct vfs_node *vnode, char *buffer, size_t nbytes)
{
  return vnode->op->read(vnode, buffer, nbytes);
}

int
vfs_write(struct vfs_node *vnode, const char *data, size_t nbytes)
{
  return vnode->op->write(vnode, data, nbytes);
}

int
vfs_close(struct vfs_node *vnode)
{
  vnode->op->close(vnode);
  free_path(&vnode->path);
  free_inode(&vnode->inode);

  return NOS_OK;
}
