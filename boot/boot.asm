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
  mov ax, LOADER_SEG
  mov es, ax

load_loader:
  ; 0x9020(ES):0x0000(BX)
  mov bx, 0
  mov ah, 0x02                    ; 读磁盘控制指令
  mov al, LOADER_SECTOR_NR        ; 读取 2 个扇区
  mov dl, 0x00                    ; 驱动器为软盘
  mov ch, 0                       ; cylinder <- 0
  mov dh, 0                       ; head <- 0
  mov cl, 2                       ; 从第二个扇区开始读（挨着 MBR）
  int 0x13                        ; 读取数据
  jnc load_loader_ok
  ; 软盘系统复位
  mov dx, 0
  mov ax, 0
  int 0x13
  jmp load_loader

load_loader_ok:
  ; 跳到 0x90200 执行 loader
  jmp LOADER_SEG:0

hang:
  hlt
  jmp hang

; 补齐到 510 个字节
; $ - $$ 为当前地址减去节的基地址
times 510-($-$$) db 0
; MBR 结尾标记
db 0x55,0xAA

; vim: ft=fasm
