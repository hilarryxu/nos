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
  printk("Page fault exception at 0x%X, error code: 0x%x\n", read_cr2(),
         tf->error_code);

  return 0;
}
