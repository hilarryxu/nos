# 不带错误码的
.macro intr_stub nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $0
    pushl $\nr
    jmp intr_common_handler
.endm

# 带错误码的
.macro intr_stub_error_code nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $\nr
    jmp intr_common_handler
.endm

# Exceptions
intr_stub 0
intr_stub 1
intr_stub 2
intr_stub 3
intr_stub 4
intr_stub 5
intr_stub 6
intr_stub 7
intr_stub_error_code 8
intr_stub 9
intr_stub_error_code 10
intr_stub_error_code 11
intr_stub_error_code 12
intr_stub_error_code 13
intr_stub_error_code 14
intr_stub 15
intr_stub 16
intr_stub_error_code 17
intr_stub 18
intr_stub 19

# IRQs
# IRQ0 : (0x20) Timer
intr_stub 32
# IRQ1 : (0x21) 键盘
intr_stub 33

# syscall
intr_stub 48

# 通用中断处理流程
.extern handle_interrupt
intr_common_handler:
    # FIXME(xcc): 后面再处理段寄存器的保存恢复
    # 目前都是运行在内核态，段寄存器没有变化

    # 入栈通用寄存器组
    # 暂时不保存恢复 esp，因为后面会非正常手段修改 esp 的值
    push %eax
    push %ecx
    push %edx
    push %ebx
    push %ebp
    push %esi
    push %edi

    # 切换为内核数据段
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es

    # 压入 struct trap_frame *tf
    push %esp
    # void handle_interrupt(tf)
    call handle_interrupt
    # 弹出 tf
    mov %eax, %esp

    # 切换为用户数据段
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es

    # 弹出通用寄存器组
    pop %edi
    pop %esi
    pop %ebp
    pop %ebx
    pop %edx
    pop %ecx
    pop %eax

    # 弹出中断号和错误码
    add $8, %esp

    # 中断返回
    iret
