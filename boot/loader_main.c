#include "nos/types.h"
#include "nos/elf.h"
#include "nos/x86.h"

void load_kernel(void);

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

void *
do_memcpy(void *dest, const void *src, int n)
{
  uchar *dest8 = (uchar*)dest;
  uchar *src8 = (uchar*)src;
  int i;

  for (i=0; i<n; i++) {
    dest8[i] = src8[i];
  }
  return dest;
}

void
load_kernel(void)
{
  void (*entry)(void);
  // void *kernel_dest = (void*)0x100000;
  void *kernel_src = (void*)0x8200;
  struct elfhdr *elf;
  struct proghdr *ph, *eph;
  uchar* pa;

  // do_memcpy(kernel_dest, kernel_src, 4096);

  elf = (struct elfhdr*)kernel_src;

  // Is this an ELF executable?
  if(elf->magic != ELF_MAGIC)
    return;  // let bootasm.S handle error

  // Load each program segment (ignores ph flags).
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
  eph = ph + elf->phnum;
  for(; ph < eph; ph++){
    pa = (uchar*)ph->paddr;
    do_memcpy(pa, (uchar *)elf + (ph->off), ph->filesz);
    if(ph->memsz > ph->filesz)
      stosb(pa + ph->filesz, 0, ph->memsz - ph->filesz);
  }

  // Call the entry point from the ELF header.
  // Does not return!
  entry = (void(*)(void))(elf->entry);
  entry();
}
