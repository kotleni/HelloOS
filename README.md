## helloos

## Building
```bash
# run to configure
apt install qemu-system-x86 gcc-multilib nasm
git clone https://github.com/kotleni/helloos
cd helloos
make init

# run to build
make
```

## Running
```bash
qemu-system-x86_64 -kernel build/kernel
```
