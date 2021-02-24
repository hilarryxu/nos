#include <nos/nos.h>
#include <nos/drvs/cga.h>
#include <nos/gdt.h>

//---------------------------------------------------------------------
// 内核主函数
//
// addr:
//   指向 multiboot 信息的结构体指针
//   struct multiboot_header *addr
//
// magic:
//   用于判断是否为 multiboot 规范启动的 magic 值
//---------------------------------------------------------------------
void
kernel_main(unsigned long addr, unsigned long magic)
{
  (void)addr;
  (void)magic;

  // 初始化 CGA 子系统
  cga_setup();
  // 初始化 GDT 全局描述符表
  gdt_setup();

  // 试下 printk
  printk("Hello nos!\n  magic=%d, addr=0x%X", magic, addr);
}
