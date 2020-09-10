/*
    Этот файл подлежит удалению,
    в будущем (возможно) перепишу тут все.
*/


// Очистить экран
void clear_screen() {
    display_clear();
}

// Вывести текст
void print(char* str) {
    display_puts(str);
}

// Вывести текст с переводом строки
void println(char* str) {
    print(str);
    display_putch('\n');
}

// Получить нажатую клавишу
KeyboardKey* readkey() {
    return keyboard_ps2_read();
}

KeyboardKey* key;
// Прочитать ввод до нажатия Enter
void readline(char* line, bool showChars) {
    key->num = -1;
    // line = malloc(sizeof(char*) * 8);
    print("\n >");
    for(;;){
        key = readkey();

        if(key->num == 0x1c) break;

        if(key->ascii != "?"){
            strcat(line, key->ascii);
            if(showChars)
                print(key->ascii);
        }
    }
}
