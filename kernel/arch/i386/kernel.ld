OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kmain)

SECTIONS
{
  . = 0x100000;

  .text : AT(0x100000) {
    *(.text .stub .text.* .gnu.linkonce.t.*)
  }
  PROVIDE(_kernel_etext = .);

  .rodata : {
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }

  /* Include debugging information in kernel memory */
  .stab : {
    PROVIDE(__STAB_BEGIN__ = .);
    *(.stab);
    PROVIDE(__STAB_END__ = .);
    BYTE(0)   /* Force the linker to allocate space for this section */
  }

  .stabstr : {
    PROVIDE(__STABSTR_BEGIN__ = .);
    *(.stabstr);
    PROVIDE(__STABSTR_END__ = .);
    BYTE(0)   /* Force the linker to allocate space for this section */
  }

  . = ALIGN(0x1000);

  .data : {
    *(.data)
  }
  PROVIDE(_kernel_edata = .);

  .bss : {
    *(.bss)
  }

  PROVIDE(_kernel_end = .);

  /DISCARD/ : {
    *(.eh_frame .note.GNU-stack)
  }
}
