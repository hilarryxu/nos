#include <nos/vga.h>

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

struct vga_text_char *vga_text_video;
struct vga_text_cursor vga_text_cursor;

static inline uint8_t
vga_text_entry_color(enum vga_text_color fg, enum vga_text_color bg)
{
  return fg | bg << 4;
}

void
vga_text_clear()
{
  vga_text_color.x = 0;
  vga_text_color.y = 0;
  vga_text_color.color = VGA_TEXT_COLOR_DEFAULT;
  memset(vga_text_video,
         (vga_text_cursor.color & 0xF0) | (vga_text_cursor.color >> 4),
         VGA_TEXT_SIZE);
}

void vga_text_printchar(char c);

void
vga_setup()
{
  vga_text_video = (struct vga_text_char *)VGA_TEXT_ADDRESS;
  vga_text_clear();
}
