#include <nos/fs/jamfs.h>

#include <string.h>

#include <nos/nos.h>

#define PATH_MAX 512

struct jamfs_file {
  struct file base;
  struct jamfs_file_header *file_header;
};

struct initrd {
  char *initrd_base;
  size_t initrd_size;
  uint32_t nfiles;
  struct jamfs_file_header *file_headers;
};

static struct initrd initrd = {
    .initrd_base = NULL, .initrd_size = 0, .nfiles = 0, .file_headers = NULL};

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
  void *src = initrd.initrd_base + p->file_header->offset + offset;
  void *start = initrd.initrd_base + p->file_header->offset;
  void *end =
      initrd.initrd_base + p->file_header->offset + p->file_header->length;
  ASSERT(src >= start && src + nbytes <= end);
  int nread = nbytes;

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

static int
jamfs_get_filesize(struct file *file, int64_t *p_size)
{
  struct jamfs_file *p = (struct jamfs_file *)file;

  if (p_size)
    *p_size = p->file_header->length;

  return NOS_OK;
}

static struct file_meths jamfs_file_meths = {.version = 1,
                                             .close = jamfs_close,
                                             .read = jamfs_read,
                                             .write = jamfs_write,
                                             .get_filesize =
                                                 jamfs_get_filesize};

static int
jamfs_open(struct vfs *vfs, const char *path, struct file *file, int flags,
           int *p_out_flags)
{
  UNUSED(vfs);

  struct jamfs_file *p = (struct jamfs_file *)file;

  if (!path)
    return -1;

  bzero(p, sizeof(*p));
  for (uint32_t i = 0; i < initrd.nfiles; i++) {
    if (strcmp(path, initrd.file_headers[i].name) == 0) {
      p->file_header = initrd.file_headers + i;
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
inird_setup(void *initrd_base, size_t initrd_size)
{
  struct jamfs_header *fs_header = (struct jamfs_header *)initrd_base;

  initrd.initrd_base = initrd_base;
  initrd.initrd_size = initrd_size;
  initrd.nfiles = fs_header->nfiles;
  initrd.file_headers =
      (struct jamfs_file_header *)(initrd_base + sizeof(*fs_header));
}
