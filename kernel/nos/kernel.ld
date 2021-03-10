ENTRY(_start)
OUTPUT_ARCH(i386)

SECTIONS
{
    /* 内核基址是 0xC0000000，.text 代码段在 0xC0100000 的位置 */
    . = 0xC0100000;

    /* 内核虚拟地址开始位置 */
    KERNEL_START = .;
    /* 内核物理地址开始位置 */
    KERNEL_START_PHYS = . - 0xC0000000;

    .text : AT(ADDR(.text) - 0xC0000000) {
        KERNEL_TEXT_START = .;
        *(.multiboot)
        *(.text)
        KERNEL_TEXT_END = .;
    }

    .data : AT(ADDR(.data) - 0xC0000000) ALIGN(4K) {
        KERNEL_DATA_START = .;
        *(.data)
        KERNEL_DATA_END = .;
    }

    .rodata : AT(ADDR(.rodata) - 0xC0000000) ALIGN(4K) {
        KERNEL_RODATA_START = .;
        *(.rodata)
        KERNEL_RODATA_END = .;
    }

    .bss : AT(ADDR(.bss) - 0xC0000000) ALIGN(4K) {
        KERNEL_BSS_START = .;
        *(COMMON)
        *(.bss)
        KERNEL_BSS_END = .;
    }

    /* 内核虚拟地址结束位置 */
    KERNEL_END = .;
    /* 内核物理地址结束位置 */
    KERNEL_END_PHYS = . - 0xC0000000;
    /* 内核镜像 */
    KERNEL_SIZE = KERNEL_END - KERNEL_START;
}
