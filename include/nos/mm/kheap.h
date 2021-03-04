#ifndef _NOS_MM_KHEAP_H
#define _NOS_MM_KHEAP_H

#include <stddef.h>

void kheap_setup();

void *kmalloc(size_t size);
void kfree(void *p);

#endif  // !_NOS_MM_KHEAP_H
