#include <kernel.h>

int kprintf(const char *fmt, ...) {
	char buf[1024] = {-1};
	va_list args;
	va_start(args, fmt);
	int out = vasprintf(buf, fmt, args);
	/* We're done with our arguments */
	va_end(args);
    display_puts("[KERN] ");
    for(int i = 0; i < out; i += 1) {
        display_putch(buf[i]);
		serial_writec(buf[i]);
    }
	return out;
}

void kpanic(const char* message) {
    display_puts("\n========[PANIC]========\n");
    display_puts(message);
    display_puts("\n=======================\n");

    for(;;) {}
}

void kassert(bool is_ok, const char* fail_message) {
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

            	display_movecur(-1, 0);
            	display_putch('\0');
            	display_movecur(-1, 0);
				break;
			default:
				char str[2];
				str[0] = key->ascii;
				str[1] = '\0';
				strcat(input, str);
            	display_putch(key->ascii);

            	cursor += 1;
				break;
		}
	}
}