#include <nos/drvs/cga.h>

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

  // 输出 hello world
  const char hw[] = "Hello World!";
  for (int i = 0; hw[i] != '\0'; i++) {
    cga_putchar(hw[i]);
  }
}
