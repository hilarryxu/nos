#include <nos/bin_loader/loader.h>

int
loader_load_image(struct process *process, uintptr_t image_start,
                  size_t image_size)
{
  if (loader_is_elf(image_start, image_size)) {
    return loader_load_elf(process, image_start, image_size);
  }

  return loader_load_flat(process, image_start, image_size);
}
