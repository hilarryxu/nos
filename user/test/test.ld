ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 2M;

    .text : {
        *(.text)
    }

    .data : ALIGN(4K) {
        *(.data)
    }

    .rodata : ALIGN(4K) {
        *(.rodata)
    }

    .bss : ALIGN(4K) {
        *(.bss)
        *(COMMON)
    }
}
