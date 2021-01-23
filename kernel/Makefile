-include ../Makefile.config

CC = $(KERNEL_CC)
CFLAGS = $(KERNEL_CFLAGS)

OBJFILES = main.o \
					 vsnprintf.o \
					 printk.o \
					 dbg.o

kernel.bin: $(OBJFILES)
	$(KERNEL_LD) -melf_i386 -T kernel.ld -o $@ $(OBJFILES)
