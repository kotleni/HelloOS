#pragma once

#include <malloc.h>

#define MAX_ROW 25
#define MAX_COL 80

#define BLACK 0x0
#define BLUE_DARK 0x1
#define GREEN_DARK 0x2
#define BLUE_LIGHT 0x3
#define RED_DARK 0x4
#define PINK 0x5
#define BROWN_LIGHT 0x6
#define GREY_LIGHT 0x7
#define DARK_GREY 0x8
#define BLUE_MID 0x9
#define GREEN_LIGHT 0xa
#define TURQUOISE 0xb
#define RED_LIGHT 0xc
#define PINK_LIGHT 0xd
#define YELLOW 0xe
#define WHITE 0xf

typedef struct {
    int col;
    int row;
    unsigned char attr;
    char *vidptr;
} Display;

void display_init();

void display_clear();

void display_putch(char c);

void display_puts(char* str);
