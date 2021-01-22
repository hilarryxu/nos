format binary
use16

include 'boot.inc'

_start:
  nop

do_print:
  ; BIOS 0x10 中断
  ; 打印字符
  mov si, msg_loader
print:
  mov al, [si]
  add si, 1
  ; 结尾的 '\x00'
  cmp al, 0
  je print_end
  mov ah, 0x0E              ; 显示字符，光标前移
  mov bx, 15                ; AL=字符 BL=前景色
  int 0x10
  jmp print
print_end:

hang:
  hlt
  jmp hang

msg_loader db '[+] loader...', 0

; 补齐到 2048 个字节，即 4 个扇区，2KB 大小
; $ - $$ 为当前地址减去节的基地址
times 2048-($-$$) db 0

; vim: ft=fasm
