nasm -f elf32 src/kernel.asm -o bin/kasm.o
gcc -m32 -ffreestanding -c src/kernel.c -o bin/kc.o
ld -m elf_i386 -T link.ld -o bin/kernel bin/kasm.o bin/kc.o
