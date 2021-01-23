BOOT_SEG = 0x07c0       ; boot 的段地址
BOOT_DRIVER_NO = 0x80   ; boot 的驱动器号

LOADER_SECTOR_NR = 8    ; loader 占用 8 个扇区（2KB + 2KB）
LOADER_SEG = 0x0000     ; loader 段地址
LOADER_OFFSET = 0x7e00  ; loader 偏移量

CR0_PE = 0x00000001

SEG_KCODE = 1
SEG_KDATA = 2
