#include <nos/fs/jamfs.h>

#include <string.h>

#include <nos/nos.h>

#define PATH_MAX 512

struct jamfs_file {
  struct file base;
  struct jamfs_file_header *file_header;
};

struct ramdisk {
  char *ramdisk_base;
  size_t ramdisk_size;
  uint32_t nfiles;
  struct jamfs_file_header *file_headers;
};

static struct ramdisk ramdisk = {
    .ramdisk_base = NULL, .ramdisk_size = 0, .nfiles = 0, .file_headers = NULL};

static int
jamfs_close(struct file *file)
{
  struct jamfs_file *p = (struct jamfs_file *)file;

  p->file_header = NULL;

  return NOS_OK;
}

static int
jamfs_read(struct file *file, void *buf, size_t nbytes, off_t offset)
{
  struct jamfs_file *p = (struct jamfs_file *)file;
  void *src = ramdisk.ramdisk_base + p->file_header->offset + offset;
  void *start = ramdisk.ramdisk_base + p->file_header->offset;
  void *end =
      ramdisk.ramdisk_base + p->file_header->offset + p->file_header->length;
  ASSERT(src >= start && src + nbytes <= end);
  int nread = offset;

  memcpy(buf, src, nbytes);

  return nread;
}

static int
jamfs_write(struct file *file, const void *buf, size_t nbytes, off_t offset)
{
  UNUSED(file);
  UNUSED(buf);
  UNUSED(nbytes);
  UNUSED(offset);

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

  struct jamfs_file *p = (struct jamfs_file *)file;

  if (!path)
    return -1;

  bzero(p, sizeof(*p));
  for (uint32_t i = 0; i < ramdisk.nfiles; i++) {
    if (strcmp(path, ramdisk.file_headers[i].name) == 0) {
      p->file_header = ramdisk.file_headers + i;
    }
  }
  if (!p->file_header)
    return -1;

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

void
inird_setup(void *ramdisk_base, size_t ramdisk_size)
{
  struct jamfs_header *fs_header = (struct jamfs_header *)ramdisk_base;

  ramdisk.ramdisk_base = ramdisk_base;
  ramdisk.ramdisk_size = ramdisk_size;
  ramdisk.nfiles = fs_header->nfiles;
  ramdisk.file_headers =
      (struct jamfs_file_header *)(ramdisk_base + sizeof(*fs_header));
}
