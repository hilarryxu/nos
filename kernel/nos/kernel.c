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

  // 输出 hello world
  const char hw[] = "Hello World!";
  int i;
  char *video = (char *)0xb8000;  // CGA 文本模式显存地址

  for (i = 0; hw[i] != '\0'; i++) {
    video[i * 2] = hw[i];     // 第一个字节为待输出字符
    video[i * 2 + 1] = 0x07;  // 第二个字节为颜色属性
  }
}
