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

/* 内核的虚拟基址 */
#define KERNEL_VIRTUAL_BASE             0xC0000000
/* 在页目录中的索引值 */
#define KERNEL_PDE_INDEX                (KERNEL_VIRTUAL_BASE >> 22)

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

# .multiboot 段
.section .multiboot

multiboot_start:
    # 16 字节对齐
    .align 16
    # 最小化的 multiboot header 定义了 magic, flags, checksum
    .long MULTIBOOT_HEADER_MAGIC
    .long MULTIBOOT_HEADER_FLAGS
    .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
multiboot_end:


# .data 数据段
.data

# 引导阶段的临时页目录（后面会变成内核页目录，复用之）
# 用了 4M 页大小特性，所以只需一级页目录就可以覆盖 4G 虚拟空间了
.global boot_pgdir
boot_pgdir:
    .align 0x1000
    # 0x83 = 0b10000011
    # bit 0: Present flag 存在位
    # bit 1: 可写
    # bit 7: 使用 4MB 页大小，而不是默认的 4KB
    # 建立对等映射 [0, 16MB) -> [0, 16MB)
    .long 0x00000083  # [0, 4MB)
    .long 0x00400083  # [4MB, 8MB)
    .long 0x00800083  # [8MB, 12MB)
    .long 0x00C00083  # [12MB, 16MB)
    # 补齐 KERNEL_PDE_INDEX 之前的页目录项
    .fill (KERNEL_PDE_INDEX - 4), 4, 0
    # 0xC0000000 虚拟地址对应的页目录项
    # 建立映射 [0xC0000000, 0xC0000000 + 4MB) -> [0, 4MB)
    .long 0x00000083
    # 总共 1024 项，补齐剩下的（4 字节的 0 值，即 0x00000000）
    .fill (1024 - KERNEL_PDE_INDEX - 1), 4, 0


# .text 代码段
.text

# 声明外部函数 kernel_main
.extern EXT_C(kernel_main)

# 同时导出 start, _start 符号，有的编译器会自动给导出的符号加下划线
# 这里两个都写出来是为了兼容加下划线和不加下划线的情况，链接脚本中使用 _start 即可
.global start, _start
start:
_start:
    # 如下这段代码在低内存地址空间加载运行（物理内存寻址）
    # 但是访问的变量确实大于 0xC0000000 的高位地址

    # 加载页目录基址到 cr3
    # 访问 boot_pgdir 需要减去个 KERNEL_VIRTUAL_BASE
    # 才可以找到其物理内存中的实际存储位置
    mov $(boot_pgdir - KERNEL_VIRTUAL_BASE), %ecx
    mov %ecx, %cr3

    # 开启 PSE(Page Size Extension)，才能使用 4MB 页大小
    # 通过置位 cr4 的 bit 4 来开启
    mov %cr4, %ecx
    or $0x00000010, %ecx
    mov %ecx, %cr4

    # 开启分页
    # 置位 cr0 的 bit 31
    mov %cr0, %ecx
    or $0x80000000, %ecx
    mov %ecx, %cr0

    # 利用间接跳转来更新 EIP，跳到高地址空间去执行
    lea _start_higher_half, %ecx
    jmp *%ecx

# 从这里开始在高端虚拟地址空间下运行
_start_higher_half:
    # 设置栈（栈由高地址往低地址生长）
    movl $(boot_kstack + STACK_SIZE), %esp
    mov $0, %ebp

    # 重置 eflags 寄存器
    pushl $0
    popf

    # 关掉中断先
    cli

    # 调用 kernel_main(unsigned long addr, unsigned long magic)
    pushl %eax                  # eax 为 multiboot magic 值
    pushl %ebx                  # ebx 为指向 multiboot_header 结构体的指针
    call EXT_C(kernel_main)     # 用到前面提到的 EXT_C 宏引用 C 语言中的函数

    # 正常 kernel_main 是永远不会返回的，这里简单处理下异常情况
    add $8, %esp
    # 关中断并进入下面的死循环
    cli

# 死循环
hang_loop:
    hlt  # 挂起 CPU
    jmp hang_loop

# boot_kstack 为栈底
# .comm 定义的符号最后会放到 .bss 段中
.comm boot_kstack, STACK_SIZE
