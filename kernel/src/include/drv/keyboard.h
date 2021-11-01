/* HelloOS */

#pragma once

#include <misc/malloc.h>
#include <misc/ports.h>

#define SC_MAX 57
#define SC_MIN 0

typedef struct {
    unsigned int num;
    char* ascii;
} KeyboardKey;

KeyboardKey* keyboard_read_unsafe();

KeyboardKey* keyboard_read();

void keyboard_init();
