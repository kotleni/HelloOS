@echo off
cls

qemu-system-i386 -vga virtio -kernel bin/kernel
rem pause