format binary as 'img'

file 'build/boot/boot.bin'
times (510)-($-$$) db 0
db 0x55,0xAA

kernel:
file 'build/kernel.bin'
times 10321920-($-$$) db 0

; vim: ft=fasm
