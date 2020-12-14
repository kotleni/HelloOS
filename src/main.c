#include <string.h>
#include <ports.h>
#include <malloc.h>

#include <display.h>
#include <keyboard.h>

void readline(char* line) {
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
    display_puts("Hello OS! v0.3\n");

    // loop
    char* line = malloc(sizeof(char*) * 64);
    for(;;) {
        memset(line, 0x00, sizeof(char*) * 64);

        display_puts("\n^p>^w ");
        readline(line);

        display_puts("\nUnknown command ");
        display_puts(line);
    }
}
