<img src="https://uniconstudio.github.io/photo_2020-09-13_14-35-18.jpg" width="200"/>

## helloos-kernel
Ядро для HelloOS<br>

<br>
## HelloOS
Попытка сделать простую операционную систему для x86 процессоров.<br/>

## ToDo
- [x] Базовое ядро
- [x] malloc (malloc.h)
- [x] Работа с портами (ports.c)
- [ ] Драйверы
    - [x] Текстовый display (display_t.h)
    - [x] PS2 Keyboard (keyboard_ps2.h)
    - [ ] PS2 Mouse
    - [ ] HDD
    - [ ] VGA
- [ ] FS
    - [ ] fat32
    - [ ] ext3
    - [ ] ntfs
- [ ] Поддержка других платформ
- [ ] Поддержка программ
- [ ] Кофиг сборки

### Запуск в qemu
```qemu-system-i386 -kernel bin/kernel.bin```

### Скриншот
![](https://github.com/qbaddev/HelloOS/raw/master/screen.jpg)
