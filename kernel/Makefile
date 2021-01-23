-include ../Makefile.config

CC = $(KERNEL_CC)
CFLAGS = $(KERNEL_CFLAGS)

OBJFILES = main.o \
					 vsnprintf.o \
					 printk.o \
					 dbg.o

kernel.bin: kernel
	$(KERNEL_OBJCOPY) -S -O binary -j .text $^ $@

kernel: $(OBJFILES)
	$(KERNEL_LD) -melf_i386 -e kmain -Ttext 0x100000 -o $@ $(OBJFILES)

kernel.objdump: kernel
	$(KERNEL_OBJDUMP) -d -S $^ > $@
