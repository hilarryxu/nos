#ifndef _NOS_DRVS_CONSOLE_H_
#define _NOS_DRVS_CONSOLE_H_

// 初始化终端
int console_setup();

void console_putc(int ch);

int console_getc();

#endif  // !_NOS_DRVS_CONSOLE_H_
