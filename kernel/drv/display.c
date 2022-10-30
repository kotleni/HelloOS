/* HelloOS */

#include <drv/display.h>
#include <misc/ports.h>

Display* display;

Display* get_display() {
    return display;
}

void display_init() {
    display = malloc(sizeof(Display*));

    display->row = 0;
    display->col = 0;
    display->attr = WHITE;
    display->vidptr = (char*)0xb8000;

    display_clear();
    disable_cursor();
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

    unsigned short pos = y * MAX_COL + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char) ((pos >> 8) & 0xFF));
}

void display_movecur(int dx, int dy) {
    display_setcur(display->col + dx, display->row + dy);
}

void enable_cursor(unsigned char cursor_start, unsigned char cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
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
