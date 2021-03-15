#include <nos/nos.h>
#include <nos/multiboot.h>
#include <nos/arch.h>
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
#include <nos/sched/sched.h>
#include <nos/fs/jamfs.h>
#include <nos/fs/vfs.h>

void kernel_main(unsigned long addr, unsigned long magic)
    __attribute__((noreturn));

extern char initrd_start[];
extern char initrd_end[];

//---------------------------------------------------------------------
// 内核主函数
//
// addr:
//   指向 multiboot 信息的结构体指针
//   struct multiboot_info *
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
  phys_addr_t aligned_free_addr =
      ALIGN_UP((phys_addr_t)KERNEL_END_PHYS, PAGE_SIZE);

  // 兼容不用 GRUB（用自带简易 bootloader）启动的方式
  struct multiboot_info *mb_info = NULL;
  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
    mb_info = (struct multiboot_info *)P2V(addr);
  }

  // NOTE: 先把串口、日志、屏幕输出等辅助调试的功能初始化好

  // 初始化串口
  serial_setup();
  // 初始化 CGA
  cga_setup();
  // 初始化调试日志输出
  // FIXME: 从启动参数中读取日志级别
  debug_setup(LOG_DEBUG);

  // 重新初始化内核页目录
  // 去掉 [0, 4MB) -> [0, 4MB) 的映射
  // 页目录的 1023 项设置为递归页目录方式（这个特性可以简化不少代码）
  aligned_free_addr = paging_setup(aligned_free_addr, mb_info);

  // 初始化物理内存管理子系统
  pmm_setup(aligned_free_addr, mb_info);
  // 初始化虚拟内存管理子系统
  vmm_setup();
  // 初始化内核堆管理子系统
  kheap_setup();
  // NOTE: 现在内核代码可以使用 kmalloc 动态分配内存了

  // 初始化 GDT
  gdt_setup();
  // 初始化异常处理
  exception_setup();
  // 初始化 PIC
  pic_setup();
  // 初始化 PIT
  pit_setup();
  // 初始化 IDT
  idt_setup();

  // 初始化 initrd 只读内存存储介质
  inird_setup(initrd_start, initrd_end - initrd_start);
  // 初始化 vfs
  vfs_setup();

  // 初始化进程子系统
  process_setup();
  // 初始化进程调度器
  sched_steup();

  // 试下 printk
  printk("Nos 0.1\n");
  printk("Kernel is %u KB large\n", ((uint32_t)KERNEL_SIZE) >> 10);
  printk("\nmagic=0x%08x, addr=0x%08x\n\n", magic, addr);

  printk("kernel : [0x%08x, 0x%08x)\n", KERNEL_START, KERNEL_END);
  printk(".text  : [0x%08x, 0x%08x)\n", KERNEL_TEXT_START, KERNEL_TEXT_END);
  printk(".data  : [0x%08x, 0x%08x)\n", KERNEL_DATA_START, KERNEL_DATA_END);
  printk(".rodata: [0x%08x, 0x%08x)\n", KERNEL_RODATA_START, KERNEL_RODATA_END);
  printk(".bss   : [0x%08x, 0x%08x)\n", KERNEL_BSS_START, KERNEL_BSS_END);
  printk("\n");

  // 试下 kmalloc
  char *p1 = kmalloc(13);
  char *p2 = kmalloc(13);
  printk("p1: %p\n", p1);
  printk("p2: %p\n", p2);

  MAGIC_BREAK();

  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
    printk("\nMultiboot:\n", mb_info->flags);
    printk("flags: 0x%x\n", mb_info->flags);
    printk("mem_lower: 0x%x\n", mb_info->mem_lower);
    printk("mem_upper: 0x%x\n", mb_info->mem_upper);
    printk("mods_addr: 0x%x\n", mb_info->mods_addr);
    printk("mods_count: %d\n", mb_info->mods_count);

    printk("mmap_addr: 0x%x\n", mb_info->mmap_addr);
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
        printk("  [0x%08x ~ 0x%08x)\n", addr, end_addr);
      }
      mmap_entry++;
    }
  }

  struct vfs *vfs = vfs_find("/");
  printk("vfs: %p\n", vfs);
  struct file *file;
  vfs_open_malloc(vfs, "test.bin", &file, 0, NULL);
  printk("file: %p\n", file);

  char buf[120];
  int64_t bin_len_b64 = 0;
  vfs_get_filesize(file, &bin_len_b64);
  int bin_len = (int)bin_len_b64;
  printk("filesize: %d\n", bin_len);
  int nread = vfs_read(file, buf, bin_len, 0);
  printk("nread: %d\n", nread);
  loga_hexdump(buf, nread, "buf [%p] with %d bytes of data", buf, nread);
  vfs_close_free(file);

  process_exec_image((uintptr_t)buf, bin_len, NULL);
  process_exec_image((uintptr_t)buf, bin_len, NULL);
  process_exec_image((uintptr_t)buf, bin_len, NULL);
  process_exec_image((uintptr_t)buf, bin_len, NULL);
  process_exec_image((uintptr_t)buf, bin_len, NULL);

  printk("[%-8s] success!\n\n", "Entry");

  // 开中断
  intr_enable();

  // 运行 idle
  cpu_idle();
}
