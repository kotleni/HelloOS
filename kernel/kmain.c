/* HelloOS */

#include <misc/string.h>
#include <misc/ports.h>
#include <misc/malloc.h>

#include <drv/display.h>
#include <drv/keyboard.h>

#include <fs/fat.h>

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
            char* path = trim(args[1]);
            FAT_File *fd = FAT_Open(path);

            if (fd->IsDirectory)
            {
                FAT_DirectoryEntry entry;
                int i = 0;
                display_putch('\n');
                while (FAT_ReadEntry(fd, &entry) && i++ < 10)
                {
                    display_puts("  ");
                    for (int i = 0; i < 11; i++)
                        display_putch(entry.Name[i]);
                    display_puts("\n");
                }
            } else {
                panic("Not implemented yet!");
            }
        } else {
            display_puts("\nUnknown command ");
            display_puts(cmd);
        }
    }
}

void panic(const char* message) {
    display_puts("\n========[PANIC]========\n");
    display_puts(message);
    display_puts("\n=======================\n");

    for(;;) {}
}

void kmain() {
    // init
    keyboard_init();
    display_init();

    display_puts(motd);
    display_puts(" ");
    display_puts(version);
    display_putch('\n');

    uint8_t identity = FAT_IdentifyAta();

    if(identity == 0) {
        panic("Can't identify ATA!");
    }

    if (!FAT_Initialize())
    {
        panic("FAT init error");
    }

    start_shell();

    // loop
    for(;;) {}
}

/*
uint32_t read = 0;
        char buffer[100];
        while ((read = FAT_Read(&disk, fd, sizeof(buffer), buffer)))
        {
            // for (uint32_t i = 0; i < read; i++)
            //     putc(buffer[i], stdout);
            // fflush(stdout);
        }*/