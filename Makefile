.phony: build

build:
	fasm boot/boot.asm
	fasm boot/loader.asm
	fasm nos.asm

clean:
	rm -f nos.img bochsout.log boot/*.bin
