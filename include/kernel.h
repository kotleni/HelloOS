#ifndef H_KERNEL
#define H_KERNEL

#include <kernel.h>

#include <drv/display.h>
#include <drv/keyboard.h>
#include <drv/serial.h>

#include <misc/string.h>
#include <misc/ports.h>
#include <misc/malloc.h>

#include <defines.h>
#include <multiboot.h>
// WARN: #include <stdio2.h>

/**
 * (Kernel) Print a formatted string.
 * %s, %c, %x, %d, %%
 *
 * @param fmt Formatted string to print
 * @param ... Additional arguments to format
 */
int kprintf(const char *fmt, ...);
void kpanic(const char* message);
void kassert(bool is_ok, const char* fail_message);
void kreadl(char* input);

#endif