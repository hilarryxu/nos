#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <drv/vga.h>
#include <kernel/ioport.h>

#define VGA_TEXT_ADDRESS 0xB8000
#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25

enum vga_text_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

#define VGA_TEXT_COLOR_DEFAULT 0x07

struct vga_text_cursor {
  int x;
  int y;
  uint8_t color;
};

struct vga_text_char {
  uint8_t chr;
  uint8_t attr;
};

#define VGA_TEXT_BPC sizeof(struct vga_text_char)
#define VGA_TEXT_SIZE (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * VGA_TEXT_BPC)
#define VGA_TEXT_TABSIZE 8

struct vga_text_char *vga_text_video;
struct vga_text_cursor vga_text_cursor = {
    .x = 0, .y = 0, .color = VGA_TEXT_COLOR_DEFAULT};

static inline uint8_t
vga_text_entry_color(enum vga_text_color fg, enum vga_text_color bg)
{
  return fg | bg << 4;
}

void
vga_text_clear()
{
  vga_text_cursor.x = 0;
  vga_text_cursor.y = 0;
  vga_text_cursor.color = VGA_TEXT_COLOR_DEFAULT;
  memset(vga_text_video,
         (vga_text_cursor.color & 0xF0) | (vga_text_cursor.color >> 4),
         VGA_TEXT_SIZE);
}

void
vga_text_set_hwcursor(int x, int y)
{
  uint16_t pos = y * VGA_TEXT_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void
vga_text_printchar(char chr)
{
  if (chr == '\b')
    vga_text_cursor.x =
        vga_text_cursor.x > 0 ? vga_text_cursor.x - 1 : vga_text_cursor.x;
  else if (chr == '\r')
    vga_text_cursor.x = 0;
  else if (chr == '\n') {
    vga_text_cursor.y++;
    vga_text_cursor.x = 0;
  } else if (chr == '\f')
    vga_text_clear();
  else if (chr == '\t')
    vga_text_cursor.x =
        vga_text_cursor.x > 0
            ? (vga_text_cursor.x / VGA_TEXT_TABSIZE + 1) * VGA_TEXT_TABSIZE
            : 0;
  else if (isprint(chr)) {
    int off = vga_text_cursor.x + vga_text_cursor.y * VGA_TEXT_WIDTH;
    if (off == 0x3FD)
      asm("hlt" ::"a"(0xD00FC0DE));
    vga_text_video[off].chr = chr;
    vga_text_video[off].attr = vga_text_cursor.color;
    vga_text_cursor.x++;
  }

  bochs_send(chr);

  if (vga_text_cursor.x >= VGA_TEXT_WIDTH) {
    vga_text_cursor.x = 0;
    vga_text_cursor.y++;
  }
  if (vga_text_cursor.y >= VGA_TEXT_HEIGHT) {
    memmove(vga_text_video, vga_text_video + VGA_TEXT_WIDTH,
            VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1) * VGA_TEXT_BPC);
    memset(vga_text_video + VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1),
           (vga_text_cursor.color & 0xF0) | (vga_text_cursor.color >> 4),
           VGA_TEXT_WIDTH * VGA_TEXT_BPC);
    vga_text_cursor.y = VGA_TEXT_HEIGHT - 1;
  }
}

void
vga_setup()
{
  vga_text_video = (struct vga_text_char *)VGA_TEXT_ADDRESS;
  vga_text_set_hwcursor(0, VGA_TEXT_HEIGHT + 1);
  vga_text_clear();
}

#define BOCHS_PORT 0xE9
void
bochs_send(char chr)
{
  outb(BOCHS_PORT, chr);
}
