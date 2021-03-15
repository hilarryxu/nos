#include <nos/drvs/cga.h>

#include <string.h>

#include <nos/ioport.h>

// 显存首地址
struct cga_char *cga_video;

// 光标
struct cga_cursor cga_cursor = {.x = 0, .y = 0, .color = CGA_COLOR_DEFAULT};

// 通过 I/O 端口编程移动光标
static void
set_hw_cursor(int x, int y)
{
  // 计算偏移
  uint16_t pos = (y * CGA_COLUMN) + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// 初始化 CGA 子系统
void
cga_setup()
{
  cga_video = (struct cga_char *)CGA_ADDRESS;
  cga_clear_screen();
}

// 清屏
void
cga_clear_screen()
{
  // 遍历清屏
  uint8_t *p = (uint8_t *)cga_video;
  for (int i = 0; i < CGA_COLUMN * CGA_LINE; i++) {
    *p++ = ' ';
    *p++ = CGA_COLOR_DEFAULT;
  }

  // 重置光标属性和位置
  cga_cursor.x = 0;
  cga_cursor.y = 0;
  cga_cursor.color = CGA_COLOR_DEFAULT;
  set_hw_cursor(0, 0);
}

// 输出字符到屏幕
void
cga_putchar(char chr)
{
  // 计算偏移
  int pos = (cga_cursor.y * CGA_COLUMN) + cga_cursor.x;

  switch (chr) {
  case '\n':
    // 处理换行
    cga_cursor.x = 0;
    cga_cursor.y++;
    break;
  case '\r':
    cga_cursor.x = 0;
    break;
  default:
    // 输出到屏幕
    cga_video[pos].chr = chr;
    cga_video[pos].attr = cga_cursor.color;
    // 水平方向前进一格
    cga_cursor.x++;
    // 判断是否到行尾了
    if (cga_cursor.x >= CGA_COLUMN) {
      // 换行
      cga_cursor.y++;
      cga_cursor.x = 0;
    }
  }

  // 滚屏
  if (cga_cursor.y >= CGA_LINE) {
    memcpy(cga_video, cga_video + (1 * CGA_COLUMN),
           sizeof(*cga_video) * CGA_LINE * CGA_COLUMN);
    cga_cursor.y = CGA_LINE - 1;
  }
}

void
cga_update_cursor()
{
  set_hw_cursor(cga_cursor.x, cga_cursor.y);
}
