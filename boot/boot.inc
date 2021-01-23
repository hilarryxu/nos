BOOT_SEG = 0x07c0       ; boot 的段地址
BOOT_DRIVER_NO = 0x00   ; boot 的驱动器号
INIT_SEG = 0x9000       ; 实模式下读到的一些硬件参数信息
LOADER_SECTOR_NR = 4    ; loader 占用 4 个扇区（2KB）
LOADER_SEG = 0x9000     ; loader 被加载到 0x90200 处运行
LOADER_OFFSET = 0x0200  ; loader 偏移量
LOADER_DS_SEG = 0x9020
