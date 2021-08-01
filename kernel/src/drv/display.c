#include <drv/display.h>

// char* vidptr = (char*)0xb8000;
Display* display;

void display_init() {
    display = malloc(sizeof(Display*));

    display->row = 0;
    display->col = 0;
    display->attr = WHITE;
    display->vidptr = (char*)0xb8000;
}

void display_clear() {
    unsigned int j = 0;
    while(j < MAX_COL * MAX_ROW * 2) {
        display->vidptr[j] = ' ';
        display->vidptr[j+1] = WHITE;
        j = j + 2;
    }

    display->col = 0;
    display->row = 0;
}

void display_setcur(int x, int y) {
    if(x > MAX_COL) { x = MAX_COL; };
    if(x < 0) { x = 0; };
    if(y > MAX_ROW) { y = MAX_ROW; };
    if(y < 0) { y = 0; };

    display->col = x;
    display->row = y;
}

void display_movecur(int dx, int dy) {
    display_setcur(display->col + dx, display->row + dy);
}

void display_putch(char c) {
    int skip = 0;
    if (c == '\n' || MAX_COL <= display->col){
        display->col = 0;

        if (display->row < MAX_ROW){
            display->row++;
        }
        skip = 1;
    }

    if (MAX_ROW <= display->row){
        display->row = 0;
        display_clear();
    }

    if (!skip){
        display->vidptr[(display->row * MAX_COL + display->col) * 2] = c;
        display->vidptr[(display->row * MAX_COL + display->col) * 2 + 1] = display->attr;
        if (display->col < MAX_COL)
        display->col++;
    }
}

void display_puts(char* str) {
    unsigned int j = 0;

    while(str[j] != '\0') {
        if(str[j] != '^'){
            display_putch(str[j]);
            ++j;
        }else{
            if(str[j+1] == 'w')
                display->attr = WHITE;
            if(str[j+1] == 'r')
                display->attr = RED_LIGHT;
            if(str[j+1] == 'p')
                display->attr = PINK_LIGHT;

            ++j;
            ++j;
        }
    }
}
