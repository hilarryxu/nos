format binary
org 7c00h
use16

_start:
  ; 初始化段寄存器和栈帧
  mov ax, 0
  mov ss, ax
  ; 栈由高地址往低地址生长
  mov sp, 0x7C00
  mov ds, ax
  mov es, ax

  ; 遍历 msg 并输出到屏幕上
  ; BIOS 0x10 中断
  mov si, msg
print:
  mov al, [si]
  add si, 1
  ; 结尾的 '\x00'
  cmp al, 0
  je print_end
  mov ah, 0x0E
  mov bx, 15
  int 0x10
  jmp print
print_end:

boot_end:
  hlt
  jmp boot_end

msg db 'hello, world!', 0

; 补齐到 510 个字节
; $ - $$ 为当前地址减去节的基地址
times 510-($-$$) db 0
; MBR 结尾标记
db 0x55,0xAA
