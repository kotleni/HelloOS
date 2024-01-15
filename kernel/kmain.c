/* HelloOS */

#include <misc/string.h>
#include <misc/ports.h>
#include <misc/malloc.h>

#include <drv/display.h>
#include <drv/keyboard.h>
#include <drv/serial.h>

#include <fs/fat.h>
#include <kernel.h>
#include <va_list.h>
#include <defines.h>
#include <multiboot.h>
#include <stdio2.h>

#define KEY_ENTER 28
#define KEY_BACKSPACE 14

void read_line(char* input) {
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

void new_shell() {
	char* input = malloc(sizeof(char) * 64);

	char abc[32];
	
	for(;;) {
		// Show shell prefix
		display_puts((char*) shell);

		// Clear input string
		memset(input, 0x00, sizeof(char) * 64);

		// Read line from input
		read_line(input);
		display_putch('\n');

		// Parse arguments
		char** args;
		int argc;
		parse_args(input, &argc, args);
        char* cmd = trim(args[0]);

		if(strcmp(cmd, "info") == 0) {

		} else {
			kern->printf("Unknown command %s!\n", args[0]);
		}
	}
}

/*
 * Integer to string
 */
static void print_dec(unsigned int value, unsigned int width, char * buf, int * ptr ) {
	unsigned int n_width = 1;
	unsigned int i = 9;
	while (value > i && i < UINT32_MAX) {
		n_width += 1;
		i *= 10;
		i += 9;
	}

	int printed = 0;
	while (n_width + printed < width) {
		buf[*ptr] = '0';
		*ptr += 1;
		printed += 1;
	}

	i = n_width;
	while (i > 0) {
		unsigned int n = value / 10;
		int r = value % 10;
		buf[*ptr + i - 1] = r + '0';
		i--;
		value = n;
	}
	*ptr += n_width;
}

/*
 * Hexadecimal to string
 */
static void print_hex(unsigned int value, unsigned int width, char * buf, int * ptr) {
	int i = width;

	if (i == 0) i = 8;

	unsigned int n_width = 1;
	unsigned int j = 0x0F;
	while (value > j && j < UINT32_MAX) {
		n_width += 1;
		j *= 0x10;
		j += 0x0F;
	}

	while (i > (int)n_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		i--;
	}

	i = (int)n_width;
	while (i-- > 0) {
		buf[*ptr + i] = "0123456789abcdef"[value & 0xF];
		value >>= 4;
	}
	*ptr += n_width;
}

/*
 * vasprintf()
 */
size_t
vasprintf(char * buf, const char *fmt, va_list args) {
	int i = 0;
	char *s;
	int ptr = 0;
	int len = strlen(fmt);
	for ( ; i < len && fmt[i]; ++i) {
		if (fmt[i] != '%') {
			buf[ptr++] = fmt[i];
			continue;
		}
		++i;
		unsigned int arg_width = 0;
		while (fmt[i] >= '0' && fmt[i] <= '9') {
			arg_width *= 10;
			arg_width += fmt[i] - '0';
			++i;
		}
		/* fmt[i] == '%' */
		switch (fmt[i]) {
			case 's': /* String pointer -> String */
				s = (char *)va_arg(args, char *);
				while (*s) {
					buf[ptr++] = *s++;
				}
				break;
			case 'c': /* Single character */
				buf[ptr++] = (char)va_arg(args, int);
				break;
			case 'x': /* Hexadecimal number */
				print_hex((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;
			case 'd': /* Decimal number */
				print_dec((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;
			case '%': /* Escape */
				buf[ptr++] = '%';
				break;
			default: /* Nothing at all, just dump it */
				buf[ptr++] = fmt[i];
				break;
		}
	}
	/* Ensure the buffer ends in a null */
	buf[ptr] = '\0';
	return ptr;

}

/**
 * (Kernel) Print a formatted string.
 * %s, %c, %x, %d, %%
 *
 * @param fmt Formatted string to print
 * @param ... Additional arguments to format
 */
int
kprintf(
		const char *fmt,
		...
	   ) {
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

int
sprintf(
		char * buf,
		const char *fmt,
		...
	   ) {
	va_list args;
	va_start(args, fmt);
	int out = vasprintf(buf, fmt, args);
	va_end(args);
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
	kern->panic(line);
}

void kernel_init() {
    kern->printf = &kprintf;
    kern->panic = &kpanic;
	kern->assert = &kassert;
}

// extern uint32_t kernel_end;
// extern uint32_t kernel_base;
extern uint8_t _kernel_base[];
extern uint8_t _malloc_base[];

void kmain(unsigned long magic, unsigned long addr) {
	mm_init(_malloc_base);
    kernel_init();
	serial_init();
    keyboard_init();
    display_init();
	fat_init();

  	/* Check multiboot magic */
  	if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
      kern->printf("Magic number is 0x%x, but expect 0x%x\n", (unsigned) magic, MULTIBOOT_BOOTLOADER_MAGIC);
      kern->panic("Invalid magic number!");
    }

	multiboot_info_t *mbi;
  	mbi = (multiboot_info_t *) addr;
	
	kern->printf("Kernel base addr: 0x%x\n", _kernel_base);
	kern->printf("Kernel end addr: 0x%x\n", _malloc_base);
	kern->printf("Available RAM: %dmb\n", mbi->mem_upper / 1024);

	FILE *file = fopen("/ETC/MOTD", "r");
	kern->assert(file != NULL, "Motd file not found!");
	char buff[256];
	fread(buff, 256, 1, file);
	display_putch('\n');
	display_puts(buff);
	display_putch('\n');
	fclose(file);

    new_shell();

	// This function not should be finished
    kern->panic("kmain is ended");
}