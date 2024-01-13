/* HelloOS */

#include <misc/string.h>
#include <misc/ports.h>
#include <misc/malloc.h>

#include <drv/display.h>
#include <drv/keyboard.h>
#include <drv/ata.h>

#include <defines.h>

void shell_input(char* input) {
    int cursor = 0;
    for(;;) {
        KeyboardKey* key = keyboard_read();

        if(key->num == 28) { // is enter
            break;
        } else if(key->num == 14) { // is backspace
            if(cursor == 0) { continue; }

            cursor -= 1;
            input[cursor] = '\0';

            display_movecur(-1, 0);
            display_putch('\0');
            display_movecur(-1, 0);
        } else { // other char
            strcat(input, key->ascii);
            display_putch(input[cursor]);
            //display_movecur(1, 0);

            cursor += 1;
        }
    }
}

void start_shell() {
    char* input = malloc(sizeof(char*) * 64);
    for(;;) {
        memset(input, 0x00, sizeof(char*) * 64); // clear input data

        display_puts((char*) shell);
        // display_setcur(sizeof(shell) + 1, get_display()->col);
        shell_input(input);                      // read input

        display_puts("\nUnknown command ");
        display_puts(input);
    }
}

void panic(const char* message) {
    display_puts("\n========[PANIC]========\n");
    display_puts(message);
    display_puts("\n=======================\n");
}

void kmain() {
    // init
    keyboard_init();
    display_init();

    display_puts(motd);
    display_puts(" ");
    display_puts(version);
    display_putch('\n');

    uint8_t identity = ata_identify();

    if(identity == 0) {
        panic("Can't identify ATA!");
        for(;;) {}
    }

    uint8_t* buff = malloc(sizeof(uint8_t) * 32);
    ata_pio_read28(0, 1, buff);

    start_shell();

    // loop
    for(;;) {}
}
