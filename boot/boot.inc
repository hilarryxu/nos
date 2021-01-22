BOOT_SEG = 0x07c0       ; boot 的段地址
INIT_SEG = 0x9000       ; 实模式下读到的一些硬件参数信息
LOADER_SECTOR_NR = 4    ; loader 占用 4 个扇区（2KB）
LOADER_SEG = 0x9020     ; loader 被加载到 0x90200 处运行
