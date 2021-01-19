.phony: build

#TOOLPREFIX = 

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -m32 -Werror -fno-omit-frame-pointer

bootblock: boot/bootasm.S boot/bootmain.c
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -o boot/bootmain.o -c boot/bootmain.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -o boot/bootasm.o -c boot/bootasm.S
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0x7C00 -o boot/bootblock.o boot/bootasm.o boot/bootmain.o
	$(OBJDUMP) -S boot/bootblock.o > boot/bootblock.asm
	$(OBJCOPY) -O binary boot/bootblock.o bootblock
