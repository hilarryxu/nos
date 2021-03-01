#ifndef _NOS_DRVS_SERIAL_H_
#define _NOS_DRVS_SERIAL_H_

#include <stdint.h>

#define SERIAL_COM1 0x3F8

void serial_setup();

char serial_read(uint16_t base);
void serial_write(uint16_t base, char chr);

#endif  // !_NOS_DRVS_SERIAL_H_
