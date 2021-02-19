#define ASM_FILE 1
#include <nos/multiboot.h>

#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/* 16KB stack */
#define STACK_SIZE                      0x4000

#ifdef __ELF__
# define AOUT_KLUDGE 0
#else
# define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
#endif
#define MULTIBOOT_HEADER_FLAGS          MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | 0


.text

.extern kmain

.p2align 2
.long MULTIBOOT_HEADER_MAGIC
.long MULTIBOOT_HEADER_FLAGS
.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.global start, _start
start:
_start:
    /* setup stack */
    movl $(stack + STACK_SIZE), %esp

    /* reset eflags */
    pushl $0
    popf

    /* call kmain(unsigned long magic, unsigned long addr) */
    pushl %ebx
    pushl %eax
    call EXT_C(kmain)

loop:
    /* halt */
    hlt
    jmp loop

.comm stack, STACK_SIZE
