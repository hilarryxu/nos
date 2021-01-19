#include "asm.h"

.code16               # 16 位模式
.globl _start
_start:
  cli                 # 屏蔽中断

  # 清零段寄存器 DS, ES, SS
  xorw %ax, %ax       # %ax = 0
  movw %ax, %ds       # Data Segment
  movw %ax, %es       # Extra Segment
  movw %ax, %ss       # Stack Segment

  # 打开 A20 地址线（用 0x92 IO 端口）
  inb $0x92, %al
  orb $0x02, %al
  outb %al, $0x92

  # 加载 GDT
  lgdt gdtdesc

  # 设置 %cr0
  movl %cr0, %eax
  orl $CR0_PE, %eax
  movl %eax, %cr0

  # 重载 %cs 和 %eip
  ljmp $(SEG_KCODE<<3), $start32

.code32               # 32 位模式
start32:
  # 重新设置段寄存器
  movw $(SEG_KDATA<<3), %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw $0, %ax
  movw %ax, %fs
  movw %ax, %gs

  # 设置栈 %esp，然后跳到 C 代码中去
  movl $_start, %esp
  call _bootmain

  # 触发 Bochs 断点
  movw $0x8a00, %ax            # 0x8a00 -> port 0x8a00
  movw %ax, %dx
  outw %ax, %dx
  movw $0x8ae0, %ax            # 0x8ae0 -> port 0x8a00
  outw %ax, %dx

  # 死循环
spin:
  jmp spin

# GDT 相关配置
.p2align 2                                # 强制 4 字节对齐（GDT 要求这样）
gdt:
  SEG_NULLASM                             # 空段
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # 代码段
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # 数据段

gdtdesc:
  .word   (gdtdesc - gdt - 1)             # sizeof(gdt) - 1
  .long   gdt                             # gdt 的地址

  . = _start+510
  .byte 0x55
  .byte 0xAA
