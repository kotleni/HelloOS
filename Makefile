TMP_DIR := /tmp
BLD_DIR := build/
INC_DIR := include/

GCC_BIN := x86_64-elf-gcc
GCC_BIN_FEDORA := gcc
LD_BIN := x86_64-elf-ld
LD_BIN_FEDORA := ld

GCC_ARGS := -m32 -I$(INC_DIR) -ffreestanding

all:
	nasm -f elf32 boot/boot.asm -o $(TMP_DIR)/boot.o
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/kmain.o -c kernel/kmain.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/display.o -c kernel/drv/display.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/keyboard.o -c kernel/drv/keyboard.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/ports.o -c kernel/misc/ports.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/malloc.o -c kernel/misc/malloc.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/string.o -c kernel/misc/string.c
	$(LD_BIN) -m elf_i386 -T link.ld -o $(BLD_DIR)/kernel $(TMP_DIR)/boot.o $(TMP_DIR)/kmain.o $(TMP_DIR)/display.o $(TMP_DIR)/keyboard.o $(TMP_DIR)/ports.o $(TMP_DIR)/malloc.o $(TMP_DIR)/string.o

fedora:
	nasm -f elf32 boot/boot.asm -o $(TMP_DIR)/boot.o
	$(GCC_BIN_FEDORA) $(GCC_ARGS) -o $(TMP_DIR)/kmain.o -c kernel/kmain.c
	$(GCC_BIN_FEDORA) $(GCC_ARGS) -o $(TMP_DIR)/display.o -c kernel/drv/display.c
	$(GCC_BIN_FEDORA) $(GCC_ARGS) -o $(TMP_DIR)/keyboard.o -c kernel/drv/keyboard.c
	$(GCC_BIN_FEDORA) $(GCC_ARGS) -o $(TMP_DIR)/ports.o -c kernel/misc/ports.c
	$(GCC_BIN_FEDORA) $(GCC_ARGS) -o $(TMP_DIR)/malloc.o -c kernel/misc/malloc.c
	$(GCC_BIN_FEDORA) $(GCC_ARGS) -o $(TMP_DIR)/string.o -c kernel/misc/string.c
	$(LD_BIN_FEDORA) -m elf_i386 -T link.ld -o $(BLD_DIR)/kernel $(TMP_DIR)/boot.o $(TMP_DIR)/kmain.o $(TMP_DIR)/display.o $(TMP_DIR)/keyboard.o $(TMP_DIR)/ports.o $(TMP_DIR)/malloc.o $(TMP_DIR)/string.o