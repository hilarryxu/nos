#include <nos/exception.h>

#include <nos/nos.h>
#include <nos/idt.h>
#include <nos/gdt.h>
#include <nos/arch.h>

typedef int (*exception_handler_pt)(struct trap_frame *);

int page_fault(struct trap_frame *tf);

exception_handler_pt handlers[32] = {
    [14] = page_fault,
};

void
exception_setup()
{
}

int
exception_dispatch(struct trap_frame *tf)
{
  int rc = -1;
  exception_handler_pt handler = handlers[tf->trap_no];

  if (handler != NULL) {
    rc = handler(tf);
  }

  return rc;
}

int
page_fault(struct trap_frame *tf)
{
  uint32_t cr2 = read_cr2();

  printk("Page fault exception at EIP: 0x%X, vaddr: 0x%X\n", tf->eip, cr2);
  printk("Error code: %b\n", tf->error_code);

  // bit 0: 为 0 表示页面不在内存中
  if (!(tf->error_code & 0x1)) {
    printk("  -> Page not present\n");
  }

  // bit 1: 为 1 写错误，为 0 读错误
  if (tf->error_code & 0x2) {
    printk("  -> Write error\n");
  } else {
    printk("  -> Read error\n");
  }

  // bit 2: 为 1 用户模式，为 0 内核模式
  if (tf->error_code & 0x4) {
    printk("  -> User mode\n");
  } else {
    printk("  -> Kernel mode\n");
  }

  // bit 3: 为 1 表示因为保留位被覆盖
  if (tf->error_code & 0x8) {
    printk("  -> Reserved bits are overwritten\n");
  }

  // bit 4: 为 1 表示访问代码，为 0 表示数据访问
  if (tf->error_code & 0x10) {
    printk("  -> Occurs when the instruction is fetched\n");
  } else {
    printk("  -> Occurs when fetching data\n");
  }

  return 0;
}
