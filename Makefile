.phony: build

build:
	as -o boot/boot.o boot/boot.s -32
	ld -T boot/boot.ld -o boot/boot.out boot/boot.o
	objcopy -O binary boot/boot.out boot.bin
	objdump -d boot/boot.out > boot/boot.disasm
