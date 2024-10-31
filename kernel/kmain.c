/* HelloOS */
#include <kernel.h>
#include <stdio2.h>

extern uint8_t _kernel_base[];
extern uint8_t _malloc_base[];
multiboot_info_t *mbi;

#define MULTIBOOT_CHECK_FLAG(flags, flag) (((flags) & (flag)) == (flag))

uint8_t *vga_buffer; // FIXME: rename
int bpp, pitch; // FIXME

void putpixel(unsigned char* screen, int x,int y, int color) {
	int pixelwidth = bpp / 8;

    unsigned where = x*pixelwidth + y*pitch;
    screen[where] = color & 255;              // BLUE
    screen[where + 1] = (color >> 8) & 255;   // GREEN
    screen[where + 2] = (color >> 16) & 255;  // RED
}

void fillrect(unsigned char *vram, unsigned char r, unsigned char g, unsigned char b, unsigned char w, unsigned char h) {
    unsigned char *where = vram;
    int i, j;

    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            putpixel(vram, j, i, (r << 16) + (g << 8) + b);
            //where[j*pixelwidth] = r;
            //where[j*pixelwidth + 1] = g;
            //where[j*pixelwidth + 2] = b;
        }
        where+=pitch;
    }
}

void run_drawlogo() {
    fillrect(vga_buffer, 255, 255, 255, 100, 100);
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
		kprintf("Framebuffer info:\n");
		kprintf("  ptr:  : 0x%x\n", mbi->framebuffer_addr);
		kprintf("  size  : %dx%d\n", mbi->framebuffer_width, mbi->framebuffer_height);
		kprintf("  pitch : %d\n", mbi->framebuffer_pitch);
		kprintf("  bpp   : %d\n", mbi->framebuffer_bpp);

		// FIX
		pitch = mbi->framebuffer_pitch;
		bpp = mbi->framebuffer_bpp;
	}

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_FRAMEBUFFER_INFO)) {
		kprintf("VBE mode: 0x%x\n", mbi->vbe_mode);
	} // TODO: Crash or use text mode? if else

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_CMDLINE)) {
		kprintf("Kernel arguments: %s\n", mbi->cmdline);
	}

	vga_buffer = (uint8_t *)mbi->framebuffer_addr;
	//memset(vga_buffer, 0xF7, 64);

	run_drawlogo();

	// FIXME: Broken after last changes
	// FILE *file = fopen("/ETC/MOTD", "r");
	// kassert(file != NULL, "Motd file not found!");
	// char buff[256];
	// fread(buff, 256, 1, file);
	// display_putch('\n');
	// display_puts(buff);
	// display_putch('\n');
	// fclose(file);

    new_shell();

	// This function not should be finished
    kpanic("kmain is ended");
}