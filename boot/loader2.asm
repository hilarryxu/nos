format ELF as 'o'

section '.text' executable

extrn loader_main

public _start
_start:
    call loader_main
    jmp $
