TMP_DIR := build/
BLD_DIR := build/
INC_DIR := include/
BASE_FS := basefs/

GCC_BIN := i686-elf-gcc
LD_BIN := i686-elf-ld

GCC_ARGS := -m32 -I$(INC_DIR) -ffreestanding -nostdlib -nostartfiles

all:
	echo Use another target instead.

qemu:
	qemu-system-x86_64 -m 512 -drive file=build/fs.img,index=0,if=ide,format=raw -bios /usr/share/ovmf/x64/OVMF_CODE.fd

pack:
	grub-mkrescue -p /boot -o build/grub.iso --modules=multiboot
	mkdir -p build/grub
	sudo mount build/grub.iso build/grub
	mkdir -p build/fs
	dd if=/dev/zero of=$(BLD_DIR)/fs.img bs=512 count=184320
	mkfs.fat -F 32 -n "DRIVE" $(BLD_DIR)/fs.img
	sudo mount build/fs.img build/fs
	
	sudo cp -r build/grub/* build/fs/
	sudo cp -r basefs/* build/fs/
	sudo cp build/kernel build/fs/boot/kernel
	sudo umount build/grub/
	sudo umount build/fs/
	sync

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