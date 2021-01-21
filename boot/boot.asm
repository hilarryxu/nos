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

  call load_loader
  jmp do_print

load_loader:
  ; 0x0000(ES):0x9000(BX)
  mov bx, 0x9000
  mov ah, 0x02      ; 读磁盘控制指令
  mov al, 2         ; 读取 2 个扇区
  mov dl, 0x00      ; 驱动器为软盘
  mov ch, 0         ; cylinder <- 0
  mov dh, 0         ; head <- 0
  mov cl, 2         ; 从第二个扇区开始读（挨着 MBR）
  int 0x13
  jc boot_end
  mov bl, 2
  cmp bl, al
  jne boot_end
  ret

do_print:
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

msg db '[+] load loader', 0

; 补齐到 510 个字节
; $ - $$ 为当前地址减去节的基地址
times 510-($-$$) db 0
; MBR 结尾标记
db 0x55,0xAA

times 256 dw 0x1234
times 256 dw 0xFACE
