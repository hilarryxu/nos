#include <nos/drvs/cga.h>

#include <nos/ioport.h>

struct cga_char *cga_video;
struct cga_cursor cga_cursor = {.x = 0, .y = 0, .color = CGA_COLOR_DEFAULT};

static void
set_hw_cursor(int x, int y)
{
  uint16_t pos = (y * CGA_WIDTH) + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void
cga_setup()
{
  cga_video = (struct cga_char *)CGA_ADDRESS;
  cga_clear_screen();
}

void
cga_clear_screen()
{
  uint8_t *p = (uint8_t *)cga_video;
  for (int i = 0; i < CGA_WIDTH * CGA_HEIGHT; i++) {
    *p++ = ' ';
    *p++ = CGA_COLOR_DEFAULT;
  }

  cga_cursor.x = 0;
  cga_cursor.y = 0;
  cga_cursor.color = CGA_COLOR_DEFAULT;
  set_hw_cursor(0, 0);
}

void
cga_putchar(char chr)
{
  int pos = (cga_cursor.y * CGA_WIDTH) + cga_cursor.x;
  cga_video[pos].chr = chr;
  cga_video[pos].attr = cga_cursor.color;
  cga_cursor.x++;
}
