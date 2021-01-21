format binary
org 7c00h
use16

_start:
  jmp $

  times 510-($-$$) db 0
  db 0x55,0xAA
