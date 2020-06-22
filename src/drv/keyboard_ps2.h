#define SC_MAX 57
#define SC_MIN 0

typedef struct KeyboardKey {
    unsigned int num;
    char* ascii;
} KeyboardKey;

// Таблица ASCII символов
const char *sc_ascii[] = { "?", "?", "1", "2", "3", "4", "5", "6",     
    "7", "8", "9", "0", "-", "=", "?", "?", "q", "w", "e", "r", "t", "y", 
        "u", "i", "o", "p", "[", "]", "?", "?", "a", "s", "d", "f", "g", 
        "h", "j", "k", "l", ";", "\"", "`", "?", "\\", "z", "x", "c", "v", 
"b", "n", "m", ",", ".", "/", "?", "?", "?", " "};

KeyboardKey* k;

// Ожидать нажатие клавиши без проверки
KeyboardKey* keyboard_ps2_read_unsafe() {
    while(!(port_byte_in(0x64) & 0x01)){
        port_byte_out(0x20 , 0x20);
    }

    k->num = port_byte_in(0x60);
    k->ascii = (char*)sc_ascii[k->num];
}

// Ожидать нажатие клавиши
KeyboardKey* keyboard_ps2_read() {
    for(;;){
        keyboard_ps2_read_unsafe();
        if(k->num <= SC_MAX && k->num > SC_MIN)
            return k;
    }
}

// Инициализировать клавиатуру
void keyboard_ps2_init() {
    port_byte_out(0x21, 0xFD);
}