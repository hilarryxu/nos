format MS COFF as 'o'

section '.text' code readable executable

extrn _loader_main

public _start
_start:
    call _loader_main
    jmp $
