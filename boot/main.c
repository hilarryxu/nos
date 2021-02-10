#include <kernel/x86.h>
#include <kernel/elf.h>

/**********************************************************************
 * This a dirt simple boot loader, whose sole job is to boot
 * an ELF kernel image from the first IDE hard disk.
 *
 * DISK LAYOUT
 *  * This program(boot.S and main.c) is the bootloader.  It should
 *    be stored in the first sector of the disk.
 *
 *  * The 2nd sector onward holds the kernel image.
 *
 *  * The kernel image must be in ELF format.
 *
 * BOOT UP STEPS
 *  * when the CPU boots it loads the BIOS into memory and executes it
 *
 *  * the BIOS intializes devices, sets of the interrupt routines, and
 *    reads the first sector of the boot device(e.g., hard-drive)
 *    into memory and jumps to it.
 *
 *  * Assuming this boot loader is stored in the first sector of the
 *    hard-drive, this code takes over...
 *
 *  * control starts in boot.S -- which sets up protected mode,
 *    and a stack so C code then run, then calls bootmain()
 *
 *  * bootmain() in this file takes over, reads in the kernel and jumps to it.
 **********************************************************************/

// 扇区大小为 512 字节
#define SECTSIZE 512
// 临时存放 ELF 文件头部的内存地址
#define ELFHDR ((struct Elf *)0x10000)  // scratch space

void readsect(void *, uint32_t);
void readseg(uint32_t, uint32_t, uint32_t);

// 加载 ELF 格式的内核文件，并运行
void
bootmain(void)
{
  struct Proghdr *ph, *eph;

  // 从硬盘读 4K 的数据到内存中（能涵盖 ELF 文件头就可以了）
  readseg((uint32_t)ELFHDR, SECTSIZE * 8, 0);

  // 判断是否是有效的 ELF 可执行文件
  if (ELFHDR->e_magic != ELF_MAGIC)
    goto bad;

  // 遍历程序段，读取磁盘中文件内容到相应内存中（忽略段标识位）
  ph = (struct Proghdr *)((uint8_t *)ELFHDR + ELFHDR->e_phoff);
  eph = ph + ELFHDR->e_phnum;
  for (; ph < eph; ph++)
    // p_pa 为待加载到内存中的物理地址
    // p_memsz 为段在内存中的大小
    // p_offset 为段在文件中偏移
    // 读取段到内存中去
    readseg(ph->p_pa, ph->p_memsz, ph->p_offset);

  // 跳转到内核入口函数执行，永远不会返回
  ((void (*)(void))(ELFHDR->e_entry))();

bad:
  outw(0x8A00, 0x8A00);
  outw(0x8A00, 0x8E00);
  while (1)
    /* do nothing */;
}

// 从内核文件 offset 偏移处开始读取 count 字节的数据到物理内存 pa 指定的内存中去
// 因为对齐的原因，实际读的数据比预期的多
void
readseg(uint32_t pa, uint32_t count, uint32_t offset)
{
  uint32_t end_pa;

  end_pa = pa + count;

  // 按 512 字节向下对齐
  pa &= ~(SECTSIZE - 1);

  // 计算扇区号（从 0 开始编号）
  // NOTE: 注意这里的 +1，因为内核紧挨着 MBR，从第二个扇区开始计算
  offset = (offset / SECTSIZE) + 1;

  // 正向依次读取，所以不必担心内存区域重叠的问题
  while (pa < end_pa) {
    // 还没开启分页，这里都是直接操作的物理地址
    readsect((uint8_t *)pa, offset);
    // 每次读一个扇区
    pa += SECTSIZE;
    offset++;
  }
}

// 等待硬盘准备好
void
waitdisk(void)
{
  while ((inb(0x1F7) & 0xC0) != 0x40)
    /* do nothing */;
}

void
readsect(void *dst, uint32_t offset)
{
  // wait for disk to be ready
  waitdisk();

  outb(0x1F2, 1);  // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // wait for disk to be ready
  waitdisk();

  // read a sector
  insl(0x1F0, dst, SECTSIZE / 4);
}
