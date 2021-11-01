/* HelloOS */

#include <misc/string.h>
#include <misc/ports.h>
#include <misc/malloc.h>

#include <drv/display.h>
#include <drv/keyboard.h>

#include <defines.h>

void readInput(char* line) {
    KeyboardKey* key;
    int cursor = 0;
    for(;;) {
        key = keyboard_read();

        if(key->num == 28)
        {
            break;
        }
        else if(key->num == 14) {
            if(cursor == 0) { continue; }

            cursor--;
            line[cursor] = '\0';

            display_movecur(-1, 0);
            display_putch('\0');
            display_movecur(-1, 0);
        }
        else {
            strcat(line, key->ascii);
            display_putch(line[cursor]);

            cursor++;
        }
    }
}

void kmain() {
    // init
    keyboard_init();
    display_init();

    // title
    display_clear();
    display_puts((char*) motd);
    display_puts("\nVersion: "); display_puts((char*) version); display_puts("\n");
    display_puts("Malloc: "); display_puts(itoa(getRamSize() / 1024)); display_puts(" MB\n");

    // loop
    char* line = malloc(sizeof(char*) * 64);
    for(;;) {
        memset(line, 0x00, sizeof(char*) * 64);

        display_puts((char*) shell);
        readInput(line);

        display_puts("\nUnknown command ");
        display_puts(line);
    }
}
