#include <nos/nos.h>
#include <nos/debug.h>
#include <nos/multiboot.h>
#include <nos/drvs/cga.h>
#include <nos/gdt.h>
#include <nos/exception.h>
#include <nos/idt.h>
#include <nos/pic.h>
#include <nos/pit.h>
#include <nos/mm/paging.h>
#include <nos/mm/pmm.h>
#include <nos/mm/vmm.h>
#include <nos/drvs/serial.h>

//---------------------------------------------------------------------
// 内核主函数
//
// addr:
//   指向 multiboot 信息的结构体指针
//   struct multiboot_info *addr
//
// magic:
//   用于判断是否为 multiboot 规范启动的 magic 值
//---------------------------------------------------------------------
void
kernel_main(unsigned long addr, unsigned long magic)
{
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    addr = 0;
  }
  struct multiboot_info *mb_info = (struct multiboot_info *)addr;

  // 初始化串口
  serial_setup();
  // 初始化调试日志输出
  debug_setup(LOG_DEBUG);

  // 初始化内核页目录并开启分页
  paging_setup();

  // 初始化 CGA
  cga_setup();

  // 初始化物理内存子系统
  pmm_setup();
  // 初始化虚拟内存子系统
  // vmm_setup();

  // 初始化 GDT
  gdt_setup();
  task_setup(mb_info);
  // 初始化异常处理
  exception_setup();
  // 初始化 PIC
  pic_setup();
  // 初始化 PIT
  pit_setup();
  // 初始化 IDT
  idt_setup();

  // 试下 printk
  printk("Hello nos!\n  magic=0x%X, addr=0x%X\n", magic, addr);

  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
    printk("\nMultiboot:\n", mb_info->flags);
    printk("flags: 0x%X\n", mb_info->flags);
    printk("mem_lower: 0x%X\n", mb_info->mem_lower);
    printk("mem_upper: 0x%X\n", mb_info->mem_upper);
    printk("mods_addr: 0x%X\n", mb_info->mods_addr);
    printk("mods_count: %d\n", mb_info->mods_count);

    printk("mmap_addr: 0x%X\n", mb_info->mmap_addr);
    printk("mmap_length: %d\n", mb_info->mmap_length);
    struct multiboot_mmap_entry *mmap_entry =
        (struct multiboot_mmap_entry *)mb_info->mmap_addr;
    struct multiboot_mmap_entry *mmap_end =
        (struct multiboot_mmap_entry *)(mb_info->mmap_addr +
                                        mb_info->mmap_length);
    while (mmap_entry < mmap_end) {
      if (mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
        uint32_t addr = mmap_entry->addr;
        uint32_t end_addr = addr + mmap_entry->len;
        printk("  [0x%X ~ 0x%X)\n", addr, end_addr);
      }
      mmap_entry++;
    }
  }

  // asm volatile("ud2");
}
