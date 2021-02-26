#include <nos/pit.h>

#include <stdint.h>

#include <nos/ioport.h>

uint32_t g_ticks = 0;

static uint32_t frequency = PIT_BASE_FREQUENCY / HZ;

void
pit_setup()
{
  outb(PIT_MODE_CMD_PORT, 0x34);

  outb(PIT_CHANNEL0_PORT, (uint8_t)(frequency & 0xFF));
  outb(PIT_CHANNEL0_PORT, (uint8_t)((frequency >> 8) & 0xFF));
}
