## helloos
Simple home made OS for x86.

## Dependecies

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

make
```

### Run on qemu
```bash
qemu-system-x86_64 -drive file=build/floppy.img,index=0,if=ide,format=raw -kernel build/kernel
```