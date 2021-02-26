#ifndef _NOS_NOS_H
#define _NOS_NOS_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

// 打印输出函数
void printk(char *format, ...);

void task_setup();

struct trap_frame *schedule(struct trap_frame *tf);

#endif  // !_NOS_NOS_H
