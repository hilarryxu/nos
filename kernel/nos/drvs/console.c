#include <nos/drvs/console.h>

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include <nos/drvs/keyboard.h>
#include <nos/drvs/cga.h>
#include <nos/sync/sync.h>

#define CONSOLE_BUF_SIZE 512

struct console {
  uint8_t buf[CONSOLE_BUF_SIZE];
  uint32_t rpos;
  uint32_t wpos;
};

static struct console cons;

static void
console_handler_key_code(uint32_t key_states, enum key_code key, void *data)
{
  UNUSED(key_states);
  UNUSED(data);
  int ch = key;
  bool intr_flag;

  local_intr_save(intr_flag);
  {
    if (isprint(ch)) {
      printk("%c", ch);
      cons.buf[cons.wpos++] = ch;
      if (cons.wpos == CONSOLE_BUF_SIZE) {
        cons.wpos = 0;
      }
    }
  }
  local_intr_restore(intr_flag);
}

struct key_code_handler console_key_code_handler = {
    .handler = console_handler_key_code,
};

int
console_setup()
{
  keyboard_set_key_code_handler(&console_key_code_handler);

  return NOS_OK;
}

void
console_putc(int ch)
{
  bool intr_flag;

  local_intr_save(intr_flag);
  {
    cga_putchar((char)ch);
  }
  local_intr_restore(intr_flag);
}

int
console_getc()
{
  int ch = 0;
  bool intr_flag;

  local_intr_save(intr_flag);
  {
    if (cons.rpos != cons.wpos) {
      ch = cons.buf[cons.rpos++];
      if (cons.rpos == CONSOLE_BUF_SIZE) {
        cons.rpos = 0;
      }
    }
  }
  local_intr_restore(intr_flag);

  return ch;
}
