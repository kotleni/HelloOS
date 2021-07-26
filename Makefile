all:
	nasm -f elf32 src/boot.asm -o bin/boot.o
	gcc -m32 -Iinclude/ -ffreestanding -o bin/main.o -c src/main.c
	gcc -m32 -Iinclude/ -ffreestanding -o bin/display.o -c src/drv/display.c
	gcc -m32 -Iinclude/ -ffreestanding -o bin/keyboard.o -c src/drv/keyboard.c
	gcc -m32 -Iinclude/ -ffreestanding -o bin/ports.o -c src/misc/ports.c
	gcc -m32 -Iinclude/ -ffreestanding -o bin/malloc.o -c src/misc/malloc.c
	gcc -m32 -Iinclude/ -ffreestanding -o bin/string.o -c src/misc/string.c
	ld -m elf_i386 -T link.ld -o bin/kernel bin/boot.o bin/main.o bin/display.o bin/keyboard.o bin/ports.o bin/malloc.o bin/string.o

run:
	qemu-system-i386 -kernel bin/kernel -m 128M

clean:
	rm -rf bin/
	mkdir bin/
