/* 定义 ASM_FILE，下面包含的头文件会根据该定义做区分 */
#define ASM_FILE 1
#include <nos/multiboot.h>

/* 有的编译器生成的 C 符号会带下划线，这里定义一个宏方便切换 */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/* 16KB 大小的栈 */
#define STACK_SIZE                      0x4000

/* 定义是 ELF 文件格式还是 a.out 格式*/
#ifdef __ELF__
# define AOUT_KLUDGE 0
#else
# define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
#endif

/*
 * 定义 multiboot 头部标志
 *
 * MULTIBOOT_PAGE_ALIGN     页对齐 multiboot_info 结构体
 * MULTIBOOT_MEMORY_INFO    multiboot_info 中包含内存信息
 */
#define MULTIBOOT_HEADER_FLAGS          MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | 0

/* .text 代码段 */
.text

/* 声明外部函数 kernel_main */
.extern EXT_C(kernel_main)

/* 按 2^2 = 4 bytes = 32 bits 对齐*/
.p2align 2
/* 最小化的 multiboot header 定义了 */
/* magic, flags, checksum */
.long MULTIBOOT_HEADER_MAGIC
.long MULTIBOOT_HEADER_FLAGS
.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

/* 同时导出 start, _start 符号，有的编译器会自动给导出的符号加下划线 */
/* 这里两个都写出来是为了兼容加下划线和不加下划线的情况，链接脚本中使用 _start 即可 */
.global start, _start
start:
_start:
    /* 设置栈（栈由高地址往低地址生长） */
    movl $(stack + STACK_SIZE), %esp

    /* 重置 eflags 寄存器 */
    pushl $0
    popf

    /* 调用 kernel_main(unsigned long addr, unsigned long magic) */
    pushl %eax                  /* eax 为 multiboot magic 值 */
    pushl %ebx                  /* ebx 为指向 multiboot_header 结构体的指针 */
    call EXT_C(kernel_main)     /* 用到前面提到的 EXT_C 宏引用 C 语言中的函数 */

/* 死循环 */
loop:
    hlt  /* 挂起 CPU */
    jmp loop

/*
 * stack 为栈底
 *
 * .comm 定义的符号最后会放到 .bss 段中
 */
.comm stack, STACK_SIZE
