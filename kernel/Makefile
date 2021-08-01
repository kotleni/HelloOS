TMP_DIR := /tmp
BLD_DIR := ../build
INC_DIR := src/include/

all:
	nasm -f elf32 src/boot.asm -o $(TMP_DIR)/boot.o
	gcc -m32 -I$(INC_DIR) -ffreestanding -o $(TMP_DIR)/main.o -c src/main.c
	gcc -m32 -I$(INC_DIR) -ffreestanding -o $(TMP_DIR)/display.o -c src/drv/display.c
	gcc -m32 -I$(INC_DIR) -ffreestanding -o $(TMP_DIR)/keyboard.o -c src/drv/keyboard.c
	gcc -m32 -I$(INC_DIR) -ffreestanding -o $(TMP_DIR)/ports.o -c src/misc/ports.c
	gcc -m32 -I$(INC_DIR) -ffreestanding -o $(TMP_DIR)/malloc.o -c src/misc/malloc.c
	gcc -m32 -I$(INC_DIR) -ffreestanding -o $(TMP_DIR)/string.o -c src/misc/string.c
	ld -m elf_i386 -T src/link.ld -o $(BLD_DIR)/kernel $(TMP_DIR)/boot.o $(TMP_DIR)/main.o $(TMP_DIR)/display.o $(TMP_DIR)/keyboard.o $(TMP_DIR)/ports.o $(TMP_DIR)/malloc.o $(TMP_DIR)/string.o
