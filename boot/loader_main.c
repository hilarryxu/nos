#include "nos/types.h"
#include "nos/x86.h"

#define SECTSIZE  512

void load_kernel(void);
void readseg(uchar*, uint, uint);

void
loader_main(void)
{
  uchar *input = (uchar *)0xB8000;
  uchar color = (0 << 4) | (15 & 0x0F);

  *input++ = 'H'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = 'l'; *input++ = color;
  *input++ = 'o'; *input++ = color;
  *input++ = ','; *input++ = color;
  *input++ = ' '; *input++ = color;
  *input++ = 'L'; *input++ = color;
  *input++ = 'o'; *input++ = color;
  *input++ = 'a'; *input++ = color;
  *input++ = 'd'; *input++ = color;
  *input++ = 'e'; *input++ = color;
  *input++ = 'r'; *input++ = color;
  *input++ = '!'; *input++ = color;

  load_kernel();
}

void
load_kernel(void)
{
  void (*entry)(void);
  void *kernel = (void*)0x10000;

  readseg((uchar*)kernel, 512, 0x1200);

  entry = (void(*)(void))(kernel);
  entry();
}

void
waitdisk(void)
{
  // Wait for disk ready.
  while((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

// Read a single sector at offset into dst.
void
readsect(void *dst, uint offset)
{
  // Issue command.
  waitdisk();
  outb(0x1F2, 1);   // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // Read data.
  waitdisk();
  insl(0x1F0, dst, SECTSIZE/4);
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void
readseg(uchar* pa, uint count, uint offset)
{
  uchar* epa;

  epa = pa + count;

  // Round down to sector boundary.
  pa -= offset % SECTSIZE;

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTSIZE) + 1;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; pa < epa; pa += SECTSIZE, offset++)
    readsect(pa, offset);
}
