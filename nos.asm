format binary as 'img'

file 'build/boot/boot'
times (510)-($-$$) db 0
db 0x55,0xAA

kernel:
file 'build/kernel/kernel.bin'
times (10*1024*1024)-($-$$) db 0

; vim: ft=fasm
