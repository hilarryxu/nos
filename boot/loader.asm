format binary
use16

include 'boot.inc'

_start:
  mov ax, LOADER_DS_SEG
  mov ds, ax

  ; 打印 loading
  mov si, msg
  call print
  jmp hang

print:
  mov ah, 0x0E
.next:
  lodsb
  or al, al
  jz .done
  int 0x10
  jmp .next
.done:
  ret

hang:
  hlt
  jmp hang

msg db '[+] loader...'
    db 13, 10, 0

; 补齐到 2048 个字节，即 4 个扇区，2KB 大小
; $ - $$ 为当前地址减去节的基地址
times 2048-($-$$) db 0

; vim: ft=fasm
