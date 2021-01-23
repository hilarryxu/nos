format binary
use16

;; 实模式下的1MB内存布局
;; 0x00000 ~ 0x003FF : (1KB) 中断向量表
;; 0x00400 ~ 0x004FF : (256B) BIOS 数据区
;; 0x00500 ~ 0x07BFF : (约 30KB) 可用区域
;; 0x07C00 ~ 0x07DFF : (512B) MBR boot
;; 0x07E00 ~ 0x9FBFF : (约 608KB) 可用区域
;; 0x9FC00 ~ 0x9FFFF : (1KB) 扩展 BIOS 数据区
;; 0xA0000 ~ 0xAFFFF : (64KB) 彩色显示适配器
;; 0xB0000 ~ 0xB7FFF : (32KB) 黑白显示适配器
;; 0xB8000 ~ 0xBFFFF : (32KB) 文本模式显示适配器
;; 0xC0000 ~ 0xFFFFF : BIOS 代码及其他数据

include 'boot.inc'

org LOADER_OFFSET

_start:
  ; 屏蔽中断
  cli

  ; 段寄存器 DS, ES, SS
  mov ax, LOADER_SEG
  mov ds, ax
  mov es, ax
  mov ss, ax

  ; 打开 A20 地址线（用 0x92 IO 端口）
  in al, 0x92
  or al, 0x02
  out 0x92, al

  ; 加载 GDT
  lgdt [gdt_desc]

  ; 设置 CR0 中的 PE 位
  mov eax, cr0
  or eax, CR0_PE
  mov cr0, eax

  ; 重载 CS 和 EIP
  jmp (SEG_KCODE shl 3):start32

use32
start32:
  ; 重新设置段寄存器
  mov ax, SEG_KDATA shl 3
  mov ds, ax
  mov es, ax
  mov ss, ax
  xor ax, ax
  mov fs, ax
  mov gs, ax

  ; 设置栈 ESP，然后跳到 C 代码中去
  mov esp, BOOT_SEG shl 4
  ; call _loader_main

spin:
  hlt
  jmp spin

align 4
gdt:
gdt_null:
  dw 0x0000
  dw 0x0000
  dw 0x0000
  dw 0x0000
gdt_kcode:
  dw 0x3fff
  dw 0x0000
  dw 0x9a00
  dw 0x00c0
gdt_kdata:
  dw 0x3fff
  dw 0x0000
  dw 0x9200
  dw 0x00c0

gdt_desc:
  dw (gdt_desc - gdt -1)
  dw gdt
  dw 0x0000

; 补齐到 2048 个字节，即 4 个扇区，2KB 大小
; $ - $$ 为当前地址减去节的基地址
times 2048-($-$$) db 0

; vim: ft=fasm
