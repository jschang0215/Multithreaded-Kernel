all:
	nasm -f bin boot.asm -o boot.bin
	dd if=message.txt >> boot.bin
	dd if=/dev/zero bs=612 count=1 >> boot.bin
	qemu-system-x86_64 -hda boot.bin