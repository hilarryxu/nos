; 刷新 GDT
; void gdt_flush(struct pseudo_desc *gdt_desc);
[GLOBAL gdt_flush]
gdt_flush:
  ; 获取 GDT 结构指针
  mov eax, [esp+4]
  ; 加载到 GDT 寄存器中
  lgdt [eax]

  ; 刷新段寄存器为内核数据段（包括堆栈段寄存器）
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  ; 刷新 CS 寄存器为内核代码段（利用长跳转实现）
  jmp 0x08:.flush
.flush:
  ret
