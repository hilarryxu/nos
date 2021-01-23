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
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0x500 -o boot/loaderblock.o boot/loader2.o boot/loader_main.o
	$(OBJDUMP) -S -M intel boot/loaderblock.o > boot/loaderblock.asm
	$(OBJCOPY) -S -O binary -j .text boot/loaderblock.o boot/loader2.bin
	# fasm nos.asm

clean:
	rm -f nos.img bochsout.log boot/*.bin
