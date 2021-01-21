.phony: build

build:
	fasm boot/boot.asm
	cp boot/boot.bin nos.img

clean:
	rm -f nos.img bochsout.log boot/*.bin
