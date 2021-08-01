## helloos

## Сборка
```bash
# выполнять только раз
apt install qemu-system-x86 gcc-multilib nasm
git clone https://github.com/kotleni/helloos
cd helloos
make init

# выполнять для каждой последующей сборки
make
```

## Запуск
```bash
qemu-system-x86_64 -kernel build/kernel
```
