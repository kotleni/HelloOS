@echo off
cls

qemu-system-i386 -vga virtio -kernel bin/kernel.bin
rem pause