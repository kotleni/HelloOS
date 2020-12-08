all:
	nasm -f elf32 src/boot.asm -o bin/boot.o
	gcc -m32 -ffreestanding -c src/main.c -o bin/main.o
	ld -m elf_i386 -T link.ld -o bin/kernel bin/boot.o bin/main.o

run:
	qemu-system-i386 -kernel bin/kernel

clean:
	rm -rf bin/
	mkdir bin/
