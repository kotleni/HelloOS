TMP_DIR := build/
BLD_DIR := build/
INC_DIR := include/
BASE_FS := basefs/

GCC_BIN := x86_64-elf-gcc
LD_BIN := x86_64-elf-ld

GCC_ARGS := -m32 -I$(INC_DIR) -ffreestanding -nostdlib -nostartfiles

all:
	echo Use another target instead.

floppy:
	dd if=/dev/zero of=$(BLD_DIR)/floppy.img bs=512 count=2880
	mkfs.fat -F 32 -n "DRIVE" $(BLD_DIR)/floppy.img
	mmd -i $(BLD_DIR)/floppy.img "::etc"
	mcopy -i $(BLD_DIR)/floppy.img $(BASE_FS)/etc/motd "::etc/motd"

base:
	nasm -f elf32 boot/boot.asm -o $(TMP_DIR)/boot.o
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/kmain.o -c kernel/kmain.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/display.o -c kernel/drv/display.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/keyboard.o -c kernel/drv/keyboard.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/serial.o -c kernel/drv/serial.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/ata.o -c kernel/drv/ata.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/ports.o -c kernel/misc/ports.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/malloc.o -c kernel/misc/malloc.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/string.o -c kernel/misc/string.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/memory.o -c kernel/misc/memory.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/fat.o -c kernel/fs/fat.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/stdio2.o -c kernel/stdio2.c
	$(GCC_BIN) $(GCC_ARGS) -o $(TMP_DIR)/kernel.o -c kernel/kernel.c
	$(LD_BIN) -m elf_i386 -T link.ld -o $(BLD_DIR)/kernel $(TMP_DIR)/boot.o $(TMP_DIR)/kmain.o $(TMP_DIR)/display.o $(TMP_DIR)/keyboard.o $(TMP_DIR)/serial.o $(TMP_DIR)/ata.o $(TMP_DIR)/ports.o $(TMP_DIR)/malloc.o $(TMP_DIR)/string.o $(TMP_DIR)/memory.o $(TMP_DIR)/fat.o $(TMP_DIR)/stdio2.o $(TMP_DIR)/kernel.o