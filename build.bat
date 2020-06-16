@echo off
cls

"C:\Program Files\NASM\nasm.exe" -f elf32 src/kernel.asm -o bin/kasm.o
gcc -m32 -ffreestanding -c src/kernel.c -o bin/kc.o
ld -m i386pe -T link.ld -o bin/kernel.bin bin/kasm.o bin/kc.o
