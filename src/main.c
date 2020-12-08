#include <string.h>
#include <ports.h>
#include <malloc.h>

#include <display.h>
#include <keyboard.h>

void readline(char* line) {
    KeyboardKey* key;
    for(;;) {
        key = keyboard_read();

        if(key->num == 28) { break; } else {
            display_puts(key->ascii);
            strcat(line, key->ascii);
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
