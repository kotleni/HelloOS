![HelloOS](https://github.com/qbaddev/HelloOS/raw/master/screen.jpg)
<br>
## HelloOS
Попытка сделать простую операционную систему для x86 процессоров.<br/>
 
>### Сделано
>* Текстовый Display драйвер (display_t.h)
>* Keyboard PS2 драйвер (keyboard_ps2.h)
>* malloc (+ free) (malloc.h)
>* Работа с портами (ports.c)

### Запуск в qemu
```qemu-system-i386 -kernel bin/kernel.bin```

<br>
<b>Подсказка: </b>В коде всегда можно получить версию с некоторыми изменениями
