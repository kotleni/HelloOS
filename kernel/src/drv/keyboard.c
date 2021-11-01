/* HelloOS */

#include <drv/keyboard.h>

// ascii table
const char *sc_ascii[] = { "?", "?", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "?", "?", "q", "w", "e", "r", "t", "y", // 14 - backspace
        "u", "i", "o", "p", "[", "]", "?", "?", "a", "s", "d", "f", "g", // 28 - enter
        "h", "j", "k", "l", ";", "\"", "`", "?", "\\", "z", "x", "c", "v",
"b", "n", "m", ",", ".", "/", "?", "?", "?", " "};

KeyboardKey* k;

// wait keypress without
KeyboardKey* keyboard_read_unsafe() {
    while(!(port_byte_in(0x64) & 0x01)){
        port_byte_out(0x20 , 0x20);
    }

    k->num = port_byte_in(0x60);
    k->ascii = (char*)sc_ascii[k->num];
}

// wait keypress
KeyboardKey* keyboard_read() {
    for(;;){
        keyboard_read_unsafe();
        if(k->num <= SC_MAX && k->num > SC_MIN)
            return k;
    }
}

// initialize keyboard
void keyboard_init() {
    port_byte_out(0x21, 0xFD);

    k = malloc(sizeof(KeyboardKey*));

    k->num = -1;
    k->ascii = (char*)"?";
}
