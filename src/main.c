#include <ports.h>
#include <malloc.h>

#include <display.h>
#include <keyboard.h>

void kmain() {
    // init
    keyboard_init();
    display_init();

    // title
    display_clear();
    display_puts("^pHello OS!^w v0.3\n");

    // loop
    KeyboardKey* key;
    for(;;) {

        key = keyboard_read();
        display_puts(key->ascii);
    }
}
