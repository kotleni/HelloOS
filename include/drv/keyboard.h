/* HelloOS */

#ifndef H_KEYBOARD
#define H_KEYBOARD

#include <misc/malloc.h>
#include <misc/ports.h>

#define KEY_ENTER 28
#define KEY_BACKSPACE 14

#define SC_MAX 57
#define SC_MIN 0

typedef struct {
    unsigned int num;
    char ascii;
} KeyboardKey;

KeyboardKey* keyboard_read_unsafe();
KeyboardKey* keyboard_read();
void keyboard_init();

#endif