#ifndef _NOS_EXCEPTION_H
#define _NOS_EXCEPTION_H

#include <nos/trap.h>

// 初始化异常处理
void exception_setup();

// 派发异常处理流程
int exception_dispatch(struct trap_frame *tf);

#endif  // !_NOS_EXCEPTION_H
