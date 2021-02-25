#include <nos/nos.h>
#include <nos/drvs/cga.h>
#include <nos/gdt.h>
#include <nos/exception.h>
#include <nos/idt.h>
#include <nos/pic.h>

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
  // 初始化 CGA
  cga_setup();
  // 初始化 GDT
  gdt_setup();
  // 初始化异常处理
  exception_setup();
  // 初始化 PIC
  pic_setup();
  // 初始化 IDT
  idt_setup();

  // 试下 printk
  printk("Hello nos!\n  magic=%X, addr=0x%X\n", magic, addr);

  // asm volatile("ud2");
}
