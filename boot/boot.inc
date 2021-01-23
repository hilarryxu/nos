BOOT_SEG = 0x07c0       ; boot 的段地址
BOOT_DRIVER_NO = 0x00   ; boot 的驱动器号

LOADER_SECTOR_NR = 4 ; loader 占用 8 个扇区（2KB + 2KB）
LOADER_SEG = 0x0900     ; loader 被加载到 0x9200 处运行
LOADER_OFFSET = 0x0200  ; loader 偏移量

CR0_PE = 0x00000001

SEG_KCODE = 1
SEG_KDATA = 2
