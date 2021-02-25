#ifndef _NOS_GDT_H
#define _NOS_GDT_H

// GDT 选择子
// (index << 3) | (ti << 2) | rpl
//
// index 为 GDT 表中序号（从 0 开始计数）
// ti(Table Indicator)
//   0 = GDT
//   1 = LDT
// rpl 请求特权级
#define KERNEL_CODE_SELECTOR 0x08  // (1 << 3) | (0 << 2) | 0 = 0x08
#define KERNEL_DATA_SELECTOR 0x10  // (2 << 3) | (0 << 2) | 0 = 0x10

// DPL 描述符特权级
enum dpl {
  DPL_0 = 0,
  DPL_1 = 1,
  DPL_2 = 2,
  DPL_3 = 3,
};

// 初始化 GDT
void gdt_setup();

#endif  // !_NOS_GDT_H