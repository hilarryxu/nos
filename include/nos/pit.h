#ifndef _NOS_PIT_H
#define _NOS_PIT_H

#include <stdint.h>

#define PIT_BASE_FREQUENCY 1193182
#define HZ 100

#define PIT_MODE_CMD_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

extern uint32_t g_ticks;

// 初始化 PIT
void pit_setup();

#endif  // !_NOS_PIT_H
