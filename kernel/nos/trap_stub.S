.global trapret
trapret:
    # ~~~ kstack
    # tf
    # trapret (kstack_context.eip) <- switch_to ret to here
    # ebp
    # ebx
    # esi
    # edi  <- kstack_context (switch_to 调整 esp0 到这里)
    # ---

    # 弹出通用寄存器组
    popal
    # 恢复段寄存器
    popl %gs
    popl %fs
    popl %es
    popl %ds

    # 弹出中断号和错误码
    addl $8, %esp

    # 中断返回
    iret

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
intr_stub 128

# 通用中断处理流程
.extern handle_interrupt
intr_common_handler:
    # 保存段寄存器
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    # 入栈通用寄存器组
    pushal

    # 切换为内核数据段
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es

    # 压入 struct trap_frame *tf
    pushl %esp
    # void handle_interrupt(tf)
    call handle_interrupt
    # 弹出 tf
    addl $4, %esp

    # 弹出通用寄存器组
    popal
    # 恢复段寄存器
    popl %gs
    popl %fs
    popl %es
    popl %ds

    # 弹出中断号和错误码
    addl $8, %esp

    # 中断返回
    iret
