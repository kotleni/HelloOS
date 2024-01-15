#ifndef H_SERIAL
#define H_SERIAL

#include <types.h>
#include <misc/ports.h>
#include <misc/string.h>

#define PORT 0x3f8 // COM1

int serial_init();
void serial_writec(char a);
void serial_write(char *str);

#endif