format binary as 'img'

file 'boot/boot'
times (510)-($-$$) db 0
db 0x55,0xAA

kernel:
file 'kernel/kernel'
times (10*1024*1024)-($-$$) db 0

; vim: ft=fasm
