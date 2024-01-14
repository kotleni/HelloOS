/* HelloOS */

#include <misc/string.h>
#include <misc/ports.h>
#include <misc/malloc.h>

#include <drv/display.h>
#include <drv/keyboard.h>

#include <fs/fat.h>
#include <kernel.h>
#include <va_list.h>
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

#define MAX_ARGS 10
#define MAX_ARG_LENGTH 50

void parse_args(const char *argsString, int *argc, char *argv[]) {
    *argc = 0;

    // Iterate through the string
    for (int i = 0; argsString[i] != '\0';) {
        // Skip leading spaces
        while (argsString[i] == ' ') {
            ++i;
        }

        // Check for the end of the string
        if (argsString[i] == '\0') {
            break;
        }

        // Find the end of the current argument
        int start = i;
        while (argsString[i] != ' ' && argsString[i] != '\0') {
            ++i;
        }
        int end = i;

        // Allocate memory for the argument and copy the substring
        argv[*argc] = malloc(end - start + 1);
        for (int j = start; j < end; ++j) {
            argv[*argc][j - start] = argsString[j];
        }
        argv[*argc][end - start] = '\0';
        ++(*argc);
    }
}

char* args[];
int argc;

void start_shell() {
    char* input = malloc(sizeof(char*) * 64);
    for(;;) {
        memset(input, 0x00, sizeof(char*) * 64); // clear input data

        display_puts((char*) shell);
        // display_setcur(sizeof(shell) + 1, get_display()->col);
        shell_input(input);                      // read input\

        parse_args(input, &argc, args);
        char* cmd = trim(args[0]);

        if(strcmp(cmd, "ls") == 0) { 
            // char* path = trim(args[1]);
            // FAT_File *fd = FAT_Open(path);

            // if (fd->IsDirectory)
            // {
            //     FAT_DirectoryEntry entry;
            //     int i = 0;
            //     display_putch('\n');
            //     while (FAT_ReadEntry(fd, &entry) && i++ < 10)
            //     {
            //         display_puts("  ");
            //         for (int i = 0; i < 11; i++)
            //             display_putch(entry.Name[i]);
            //         display_puts("\n");
            //     }
            // } else {
            //     panic("Not implemented yet!");
            // }

            // FAT_Close(fd);
        } else {
            display_puts("\nUnknown command ");
            display_puts(cmd);
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
		buf[*ptr] = "0123456789abcdef"[(value>>(i*4))&0xF];
		*ptr += + 1;
	}
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
    for(int i = 0; i < out; i += 1) {
        display_putch(buf[i]);
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

void kernel_init() {
    kern = (kernel*) malloc(sizeof(kernel));
    kern->printf = &kprintf;
    kern->panic = &kpanic;
}

void kmain() {
    kernel_init();
    keyboard_init();
    display_init();

    kern->printf("value is %d\n", 32);

    start_shell();

    // loop
    for(;;) {}
}