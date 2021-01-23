format binary
org 7c00h
use16

include 'boot.inc'

_start:
  ; 初始化段寄存器和栈帧
  mov ax, 0
  mov ss, ax
  ; 栈由高地址往低地址生长
  mov sp, BOOT_SEG
  mov ds, ax

  ; 打印 loading
  mov si, msg
  call print

load_loader:
  mov ax, LOADER_SEG
  mov es, ax
  mov bx, LOADER_OFFSET
  mov ah, 0x02                    ; 读磁盘控制指令
  mov dl, BOOT_DRIVER_NO          ; 驱动器为软盘
  mov ch, 0                       ; cylinder <- 0
  mov dh, 0                       ; head <- 0
  mov cl, 2                       ; 从第二个扇区开始读（挨着 MBR）
  mov al, LOADER_SECTOR_NR        ; 读取多少个扇区
  int 0x13                        ; 读取数据
  jnc load_loader_ok
  ; 软盘系统复位
  mov dx, 0
  mov ax, 0
  int 0x13
  jmp load_loader

load_loader_ok:
  ; 跳到 0x90200 执行 loader
  jmp LOADER_SEG:LOADER_OFFSET

hang:
  hlt
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

msg db '[+] boot...'
    db 13, 10, 0

; 补齐到 510 个字节
; $ - $$ 为当前地址减去节的基地址
times 510-($-$$) db 0
; MBR 结尾标记
db 0x55,0xAA

; vim: ft=fasm
