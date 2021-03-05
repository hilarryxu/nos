#ifndef _NOS_DRVS_CGA_H_
#define _NOS_DRVS_CGA_H_

//=====================================================================
// VGA 是基于 MDA,CGA,EGA 的，有很多模式
//
// BIOS 默认启动时使用的是模式 7，Color text mode and CGA
// 就是 80 列，25 行的几种颜色的文本模式，我这里就简称为 cga 了
//
// 后面还有高级的图形模式，以及 Vesa BIOS Extensions (VBE) 等概念
// 暂且不去关心，还是回到操作系统本身
//=====================================================================

#include <stdint.h>

#include <nos/mm/memlayout.h>

// 显示最小单元
struct cga_char {
  uint8_t chr;   // 字符
  uint8_t attr;  // 颜色属性
};

// 光标
struct cga_cursor {
  int x;          // 行
  int y;          // 列
  uint8_t color;  // 当前颜色
};

// 该模式下的显存地址（虚拟地址空间下）
#define CGA_ADDRESS (KERNEL_BASE + 0xB8000)
// 总共 80 列
#define CGA_COLUMN 80
// 总共 25 行
#define CGA_LINE 25

// 默认颜色
#define CGA_COLOR_DEFAULT 0x07

// 初始化 CGA 子系统
void cga_setup();
// 清屏
void cga_clear_screen();
// 输出字符到屏幕
void cga_putchar(char chr);

#endif  // !_NOS_DRVS_CGA_H_
