#ifndef _NOS_GDT_H
#define _NOS_GDT_H

// GDT 选择子
#define KERNEL_CODE_SELECTOR 0x8
#define KERNEL_DATA_SELECTOR 0x10

// DPL 特权级
enum dpl {
  DPL_0 = 0,
  DPL_1 = 1,
  DPL_2 = 2,
  DPL_3 = 3,
};

// 初始化 GDT
void gdt_setup();

#endif  // !_NOS_GDT_H
