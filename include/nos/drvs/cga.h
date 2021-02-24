#ifndef _NOS_DRVS_CGA_H_
#define _NOS_DRVS_CGA_H_

#include <stdint.h>

struct cga_char {
  uint8_t chr;
  uint8_t attr;
};

struct cga_cursor {
  int x;
  int y;
  uint8_t color;
};

#define CGA_ADDRESS 0xB8000
#define CGA_WIDTH 80
#define CGA_HEIGHT 25

#define CGA_COLOR_DEFAULT 0x07

void cga_setup();
void cga_clear_screen();
void cga_putchar(char chr);

#endif  // !_NOS_DRVS_CGA_H_
