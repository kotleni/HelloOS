all:
	make -C kernel/
	#make -C bootloader/

init:
	mkdir build

run:
	qemu-system-x86_64 -kernel build/kernel
