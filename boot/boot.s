.code16
.section .text
.globl _start
_start:
    movb $65, %al
    movb $0x0E, %ah
    movb $0x00, %bh
    movb $0x07, %bl
    int $0x10
_hang:
    jmp _hang
    . = _start+510
    .byte 0x55
    .byte 0xAA
