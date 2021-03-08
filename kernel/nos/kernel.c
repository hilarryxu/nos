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
#include <nos/mm/kheap.h>
#include <nos/drvs/serial.h>
#include <nos/proc/scheduler.h>

//---------------------------------------------------------------------
// 内核主函数
//
// addr:
//   指向 multiboot 信息的结构体指针
//   multiboot_info_t *
//   是处在内存低端的物理地址，所以用 P2V 转换成虚拟地址再访问
//
// magic:
//   用于判断是否为 multiboot 规范启动的 magic 值
//---------------------------------------------------------------------
void
kernel_main(unsigned long addr, unsigned long magic)
{
  // 计算内核尾部开始的可用物理地址（不得超过 4MB，需要页对齐）
  // TODO: 这里直接用的 KERNEL_END_PHYS，后面考虑 multiboot mods 时需重新计算
  phys_addr_t free_addr = ALIGN_UP((phys_addr_t)KERNEL_END_PHYS, PAGE_SIZE);

  // 兼容不用 GRUB（用自带简易 bootloader）启动的方式
  multiboot_info_t *mb_info = NULL;
  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
    mb_info = (multiboot_info_t *)P2V(addr);
  }

  // Note: 先把串口、日志、屏幕输出等辅助调试的功能初始化好

  // 初始化串口
  serial_setup();
  // 初始化调试日志输出
  debug_setup(LOG_DEBUG);
  // 初始化 CGA
  cga_setup();

  // 重新初始化内核页目录
  // 去掉 [0, 4MB) -> [0, 4MB) 的映射
  // 页目录的 1023 项设置为递归页目录方式（这个特性可以简化不少代码）
  paging_setup();

  // 初始化物理内存管理子系统
  pmm_setup(free_addr);
  // 初始化虚拟内存管理子系统
  vmm_setup();
  // 初始化内核堆管理子系统
  kheap_setup();
  // Note: 内核代码可以使用 kmalloc 动态分配内存了

  // 初始化 GDT
  gdt_setup();
  // task_setup(mb_info);
  // 初始化异常处理
  exception_setup();
  // 初始化 PIC
  pic_setup();
  // 初始化 PIT
  pit_setup();
  // 初始化 IDT
  idt_setup();

  // 初始化进程调度器
  scheduler_steup();

  // 试下 printk
  printk("Nos 0.1\n");
  printk("Kernel is %d KB large\n", ((uint32_t)KERNEL_SIZE) >> 10);
  printk("\nmagic=0x%X, addr=0x%X\n", magic, addr);

  char *p1 = kmalloc(13);
  char *p2 = kmalloc(13);
  printk("p1: 0x%X\n", p1);
  printk("p2: 0x%X\n", p2);

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
        (struct multiboot_mmap_entry *)CAST_P2V(mb_info->mmap_addr);
    struct multiboot_mmap_entry *mmap_end =
        (struct multiboot_mmap_entry *)CAST_P2V(mb_info->mmap_addr +
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

  // scheduler();
}
