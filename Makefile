-include Makefile.config

.phony: all

CC = $(KERNEL_CC)
CFLAGS = $(KERNEL_CFLAGS)

all:
	fasm boot/boot.asm
	fasm boot/loader.asm
	fasm boot/loader2.asm
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I./include -o boot/loader_main.o -c boot/loader_main.c
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0x8000 -o boot/loaderblock.o boot/loader2.o boot/loader_main.o
	$(KERNEL_OBJDUMP) -d -S boot/loaderblock.o > boot/loaderblock.asm
	$(KERNEL_OBJCOPY) -S -O binary -j .text boot/loaderblock.o boot/loader2.bin

	# $(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -o kernel/kernel.o -c kernel/kernel.c
	# $(LD) $(LDFLAGS) -N -e _kmain -Ttext 0x100000 -o kernel/kernelblock.o kernel/kernel.o
	# $(OBJDUMP) -S -M intel kernel/kernelblock.o > kernel/kernelblock.asm
	# $(OBJCOPY) -S -O binary -j .text kernel/kernelblock.o kernel/kernel.bin
	make -C kernel

	fasm nos.asm

clean:
	rm -f nos.img bochsout.log boot/*.bin boot/*.o
