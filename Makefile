.phony: build

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -m32 -Werror -fno-omit-frame-pointer -I./include/

build:
	fasm boot/boot.asm
	fasm boot/loader.asm
	fasm boot/loader2.asm
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -o boot/loader_main.o -c boot/loader_main.c
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0x8600 -o boot/loaderblock.o boot/loader2.o boot/loader_main.o
	$(OBJDUMP) -S -M intel boot/loaderblock.o > boot/loaderblock.asm
	$(OBJCOPY) -S -O binary -j .text boot/loaderblock.o boot/loader2.bin

	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -o kernel/kernel.o -c kernel/kernel.c
	$(LD) $(LDFLAGS) -N -e _kmain -Ttext 0x10000 -o kernel/kernelblock.o kernel/kernel.o
	$(OBJDUMP) -S -M intel kernel/kernelblock.o > kernel/kernelblock.asm
	$(OBJCOPY) -S -O binary -j .text kernel/kernelblock.o kernel/kernel.bin

	fasm nos.asm

	dd if=nos.img of=c.img bs=8704 count=1 conv=notrunc

clean:
	rm -f nos.img bochsout.log boot/*.bin boot/*.o
