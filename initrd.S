.data

.global initrd_start
initrd_start:
    .incbin "build/initrd"
.global initrd_end
initrd_end:
