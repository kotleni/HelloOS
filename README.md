## helloos
Simple home made OS for x86.

<img src="https://github.com/kotleni/HelloOS/assets/38311102/dcfb5f74-3182-4dfe-996d-967ecda52471" width=370>

### Whats working?
- [x] Memory allocator
- [x] Multiboot loader support
- [x] Text mode video driver
- [x] ATA driver
- [x] Keyboard driver
- [ ] Serial driver
- [x] FAT32 filesystem

### Dependecies
Ubuntu:
```bash
apt install qemu-system-x86 gcc-multilib nasm
```

Fedora:
```bash
dnf install qemu glibc-devel nasm
```

MacOS:
```bash
brew install x86_64-elf-gcc nasm qemu-system-x86
```

### Build
```bash
git clone https://github.com/kotleni/helloos
cd helloos
mkdir build

make floppy
make
```

### Run on qemu
```bash
qemu-system-x86_64 -m 512 -drive file=build/floppy.img,index=0,if=ide,format=raw -kernel build/kernel
```
<i>** Floppy driver using as root fs drive.</i>