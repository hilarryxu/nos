#ifndef _NOS_DRVS_SERIAL_H_
#define _NOS_DRVS_SERIAL_H_

#include <stdint.h>

#define SERIAL_COM1 0x3F8

// 默认波特率
#define SERIAL_DEFAULT_BAUDRATE 9600

// 初始化串口
void serial_setup();

// 读串口
char serial_read(uint16_t base);
// 写串口
void serial_write(uint16_t base, char chr);

#endif  // !_NOS_DRVS_SERIAL_H_
