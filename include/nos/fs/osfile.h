#ifndef _NOS_FS_OSFILE_H
#define _NOS_FS_OSFILE_H

#include <nos/fs/vfs.h>

struct osfile {
  struct file *f_file;
  off_t offset;
  int refcount;
};

#endif  // !_NOS_FS_OSFILE_H
