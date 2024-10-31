/* HelloOS */
#include <kernel.h>
#include <stdio2.h>

extern uint8_t _kernel_base[];
extern uint8_t _malloc_base[];
multiboot_info_t *mbi;

#define MULTIBOOT_CHECK_FLAG(flags, flag) (((flags) & (flag)) == (flag))

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_ADDRESS 0xA0000 // VGA framebuffer address

void draw_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return; // Out of bounds
    }
    uint8_t *vga_buffer = (uint8_t *)VGA_ADDRESS;
    vga_buffer[y * VGA_WIDTH + x] = color; // Set pixel color
}

void draw_rectangle(int x, int y, int width, int height, uint8_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

void run_drawlogo() {
    draw_rectangle(50, 50, 100, 50, 4);
}

void new_shell() {
	char* input = malloc(sizeof(char) * 64);

	char abc[32];
	
	for(;;) {
		// Show shell prefix
		display_puts((char*) SHELL_PROMPT);

		// Clear input string
		memset(input, 0x00, sizeof(char) * 64);

		// Read line from input
		kreadl(input);
		display_putch('\n');

		// Parse arguments
		char** args;
		int argc;
		parse_args(input, &argc, args);
        char* cmd = trim(args[0]);

		if(strcmp(cmd, "drawlogo") == 0) {
			run_drawlogo();
		} else {
			kprintf("Unknown command %s!\n", args[0]);
		}
	}
}

void kmain(unsigned long magic, unsigned long addr) {
	mm_init((uint32_t)_malloc_base);
	serial_init();
    keyboard_init();
    display_init();
	fat_init();

  	/* Check multiboot magic */
  	if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
      kprintf("Magic number is 0x%x, but expect 0x%x\n", (unsigned) magic, MULTIBOOT_BOOTLOADER_MAGIC);
      kpanic("Invalid magic number!");
    }

  	mbi = (multiboot_info_t *) addr;
	
	kprintf("[BOOTLOADER REPORTED INFO]\n");

	kprintf("Bootloader: %s\n", mbi->boot_loader_name);
	kprintf("Boot device: %s\n", mbi->boot_device);

	kprintf("Kernel base addr: 0x%x\n", _kernel_base);
	kprintf("Kernel end addr: 0x%x\n", _malloc_base);

	kprintf("Available RAM: %dmb\n", mbi->mem_upper / 1024);

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_FRAMEBUFFER_INFO)) {
		kprintf("Framebuffer addr: 0x%x, type: %x, size: %dx%d\n", mbi->framebuffer_addr, mbi->framebuffer_type, mbi->framebuffer_width, mbi->framebuffer_height);
	}

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_FRAMEBUFFER_INFO)) {
		kprintf("VBE mode: 0x%x\n", mbi->vbe_mode);
	}

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_CMDLINE)) {
		kprintf("Kernel arguments: %s\n", mbi->cmdline);
	}

	// unsigned char *framebuffer = (void*)mbi->framebuffer_addr;
	// framebuffer[8 * VGA_WIDTH + 8] = 4;

	FILE *file = fopen("/ETC/MOTD", "r");
	kassert(file != NULL, "Motd file not found!");
	char buff[256];
	fread(buff, 256, 1, file);
	display_putch('\n');
	display_puts(buff);
	display_putch('\n');
	fclose(file);

	run_drawlogo();
    new_shell();

	// This function not should be finished
    kpanic("kmain is ended");
}