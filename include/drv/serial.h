#pragma once

#include <types.h>
#include <misc/ports.h>

#define PORT 0x3f8 // COM1

int serial_init();
void serial_writec(char a);
void serial_write(char *str);