format binary as 'img'

file 'boot/boot.bin'
file 'boot/loader.bin'
file 'boot/loader2.bin'

times (9 * 512)-($-$$) db 0

; vim: ft=fasm
