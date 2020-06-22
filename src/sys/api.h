void clear_screen() {
    display_clear();
}

void print(char* str) {
    display_puts(str);
}

void println(char* str) {
    print(str);
    display_putch('\n');
}

KeyboardKey* readkey() {
    return keyboard_ps2_read();
}