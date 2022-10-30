## helloos
Simple home made OS for x86.

## Building

Ubuntu:
```bash
# run to prepare
apt install qemu-system-x86 gcc-multilib nasm
git clone https://github.com/kotleni/helloos
cd helloos
make init

# run to build
make

# run to test
qemu-system-x86_64 -kernel build/kernel
```

MacOS:
```bash
# run to prepare
brew install x86_64-elf-gcc
brew install nasm
brew install qemu-system-x86
git clone https://github.com/kotleni/helloos
cd helloos

# run to build
mkdir build
make

# run to test
qemu-system-x86_64 -kernel build/kernel
```
