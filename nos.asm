format binary as 'img'

file 'boot/boot.bin'
file 'boot/loader.bin'
file 'boot/loader2.bin'

times (3 * 512)-($-$$) db 0

kernel:
file 'kernel/kernel.bin'
times (4096)-($-kernel) db 0

; vim: ft=fasm
