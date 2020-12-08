all:
	nasm -f elf32 src/boot.asm -o bin/boot.o
	gcc -m32 -Iinclude/ -ffreestanding -o bin/main.o -c src/main.c
	gcc -m32 -Iinclude/ -ffreestanding -o bin/display.o -c src/drv/display.c
	ld -m elf_i386 -T link.ld -o bin/kernel bin/boot.o bin/main.o bin/display.o

run:
	qemu-system-i386 -kernel bin/kernel

clean:
	rm -rf bin/
	mkdir bin/
