#ifndef _NOS_IOPORT_H
#define _NOS_IOPORT_H

#include <stdint.h>

// 单字节读 IO 端口数据
static inline uint8_t
inb(uint16_t _port)
{
  uint8_t _result;
  asm volatile("inb %1, %0" : "=a"(_result) : "Nd"(_port));
  return _result;
}

// 单字节写 IO 端口数据
static inline void
outb(uint16_t _port, uint8_t _data)
{
  asm volatile("outb %0, %1" ::"a"(_data), "Nd"(_port));
}

#endif  // !_NOS_IOPORT_H
