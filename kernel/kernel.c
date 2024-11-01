#include <kernel.h>

int chx = 1;
int chy = 1;

void _putch(char ch) {
	_putchnoswap(ch);
	canvas_swap();
}

void _putchnoswap(char ch) {
	if(ch == '\n') {
		_newline();
		return;
	}

	canvas_drawchar(ch, chx, chy, 0xFFFFFF, 0x000000);
	chx++;
	// TODO
}

void _puts(char *str) {
	int len = strlen(str);
	for(int i = 0; i < len; i++) {
		_putchnoswap(str[i]);
	}
	canvas_swap();
}

void _resetline() {
	chx = 0;
	chy = 0;
}

void _movecursor(int x, int y) {
	chx = x;
	chy = y;
}

void _newline() {
	chx = 0;
	chy++;
}

void _clearscreen() {
	// TODO: Rewrite to memset?
	for(int x = 0; x < canvas.width; x++) {
		for(int y = 0; y < canvas.height; y++) {
			canvas_drawpixel(x, y, 0x000000);
		}
	}
	canvas_swap();
}

int printf(char *fmt, ...) {
	char buf[1024] = {-1};
	va_list args;
	va_start(args, fmt);
	int out = vasprintf(buf, fmt, args);
	/* We're done with our arguments */
	va_end(args);
    for(int i = 0; i < out; i += 1) {
        _putch(buf[i]);
		serial_writec(buf[i]);
    }
	return out;
}

int kprintf(char *fmt, ...) {
	_puts("[KERN] ");
	return printf(fmt);
}

void kpanic(char* message) {
    _puts("\n========[PANIC]========\n");
    _puts(message);
    _puts("\n=======================\n");

    for(;;) {}
}

void kassert(bool is_ok, char* fail_message) {
	if(is_ok) return;

	char title[] = "ASSERT FAIED:\n";

	int msg_len = strlen(fail_message);
	char* line = malloc((sizeof(char) * msg_len) + sizeof(title));
	strcat(line, "ASSERT FAIED:\n");
	strcat(line, fail_message);
	kpanic(line);
}

void kreadl(char* input) {
	int cursor = 0;
	KeyboardKey* key;
	for(;;) {
		key = keyboard_read();

		switch(key->num) {
			case KEY_ENTER:
				return;
			case KEY_BACKSPACE:
				if(cursor == 0) continue;

            	cursor -= 1;
            	input[cursor] = '\0';

            	//display_movecur(-1, 0);
            	_putch('\0');
            	//display_movecur(-1, 0);
				break;
			default:
				char str[2];
				str[0] = key->ascii;
				str[1] = '\0';
				strcat(input, str);
            	_putch(key->ascii);

            	cursor += 1;
				break;
		}
	}
}