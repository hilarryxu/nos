#ifndef _NOS_FS_JAMFS_H
#define _NOS_FS_JAMFS_H

#include <nos/fs/vfs.h>

struct jamfs_header {
  uint32_t nfiles;
};

struct jamfs_file_header {
  uint8_t magic;
  char name[64];
  uint32_t offset;
  uint32_t length;
};

#endif  // !_NOS_FS_JAMFS_H
