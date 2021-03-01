#include <nos/drvs/serial.h>

#include <nos/ioport.h>

#define SERIAL_DEFAULT_BAUDRATE 9600

#define SERIAL_REG_DATA 0  // Data register
#define SERIAL_REG_IER 1   // Interrupt Enable Register
#define SERIAL_REG_IIR 2   // Interrupt Identification
#define SERIAL_REG_FCR 2   // FIFO control registers
#define SERIAL_REG_LCR 3   // Line Control Register
#define SERIAL_REG_MCR 4   // Modem Control Register
#define SERIAL_REG_LSR 5   // Line Status Register

void
serial_setup()
{
  uint16_t base = SERIAL_COM1;
  // uint16_t divisor = 115200 / SERIAL_DEFAULT_BAUDRATE;

  outb(base + SERIAL_REG_IER, 0x00);  // 关闭所有中断
  // 置位 DLAB (接下来可以设置 baud rate divisor)
  outb(base + SERIAL_REG_LCR, 0x80);
  outb(base + SERIAL_REG_DATA, 0x03);  // 设置 divisor 为 3 (lo byte) 38400 baud
  outb(base + SERIAL_REG_IER, 0x00);  //                    (hi byte)
  // 清零 DLAB，并设置模式: 8 bits, no parity, one stop bit
  outb(base + SERIAL_REG_LCR, 0x03);
  // 参见 https://www.lookrs232.com/rs232/fcr.htm
  // Enable FIFO, clear them, with 14-byte threshold
  outb(base + SERIAL_REG_FCR, 0xC7);
  // 参见 https://www.lookrs232.com/rs232/mcr.htm
  outb(base + SERIAL_REG_MCR, 0x0B);  // IRQs enabled, RTS/DSR set

  // 回环模式下测试读写是否正常
  outb(base + SERIAL_REG_MCR, 0x1E);  // 设置为 loopback 模式
  outb(base + 0, 0xAE);               // 发送一个字节的测试数据
  if (inb(base + 0) != 0xAE) {
    // FIXME(xcc): print err
    return;
  }

  // 打开中断，置位 RTS/DSR
  outb(base + SERIAL_REG_MCR, 0x0F);
}

static int
is_received(uint16_t base)
{
  return inb(base + SERIAL_REG_LSR) & 1;
}

char
serial_read(uint16_t base)
{
  while (is_received(base) == 0)
    ;

  return inb(base + SERIAL_REG_DATA);
}

static int
is_transmit_empty(uint16_t base)
{
  return inb(base + SERIAL_REG_LSR) & 0x20;
}

void
serial_write(uint16_t base, char chr)
{
  while (is_transmit_empty(base) == 0)
    ;

  outb(base + SERIAL_REG_DATA, chr);
}
