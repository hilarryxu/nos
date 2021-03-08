# switch_to(struct kstack_context **old, struct kstack_context *new);
.global switch_to
switch_to:
    # eax = old
    # edx = new
    movl 4(%esp), %eax
    movl 8(%esp), %edx

    # 保存旧的 callee-saved registers（旧内核栈）
    pushl %ebp
    pushl %ebx
    pushl %esi
    pushl %edi

    # 切换内核栈
    # *old = esp
    # esp = new
    movl %esp, (%eax)
    movl %edx, %esp

    # 加载新的 callee-saved registers（新内核栈）
    popl %edi
    popl %esi
    popl %ebx
    popl %ebp
    ret
