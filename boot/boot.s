.code16
.section .text
.globl _start
_start:
    jmp _boot
    nop
_boot:
    jmp _boot
    . = _start+510
    .byte 0x55
    .byte 0xAA
