#ifndef _NOS_BIN_LOADER_LOADER_H
#define _NOS_BIN_LOADER_LOADER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct process;

int loader_load_image(struct process *process, uintptr_t image_start,
                      size_t image_size);

int loader_load_flat(struct process *process, uintptr_t image_start,
                     size_t image_size);

bool loader_is_elf(uintptr_t image_start, size_t image_size);

int loader_load_elf(struct process *process, uintptr_t image_start,
                    size_t image_size);

#endif  // !_NOS_BIN_LOADER_LOADER_H
