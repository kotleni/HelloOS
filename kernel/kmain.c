/* HelloOS */
#include <kernel.h>
#include <stdio2.h>

extern uint8_t _kernel_base[];
extern uint8_t _malloc_base[];
multiboot_info_t *mbi;

#define MULTIBOOT_CHECK_FLAG(flags, flag) (((flags) & (flag)) == (flag))

void print_bootinfo() {
	printf("[BOOTLOADER REPORTED INFO]\n");

	printf("Bootloader: %s\n", mbi->boot_loader_name);

	printf("Kernel base addr: 0x%x\n", _kernel_base);
	printf("Kernel end addr: 0x%x\n", _malloc_base);

	printf("Available RAM: %dmb\n", mbi->mem_upper / 1024);

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_FRAMEBUFFER_INFO)) {
		printf("Framebuffer info:\n");
		printf("  ptr:  : 0x%x\n", mbi->framebuffer_addr);
		printf("  size  : %dx%d\n", mbi->framebuffer_width, mbi->framebuffer_height);
		printf("  pitch : %d\n", mbi->framebuffer_pitch);
		printf("  bpp   : %d\n", mbi->framebuffer_bpp);
	}

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_FRAMEBUFFER_INFO)) {
		printf("VBE mode: 0x%x\n", mbi->vbe_mode);
	}

	if(MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_INFO_CMDLINE)) {
		printf("Kernel arguments: %s\n", mbi->cmdline);
	}
}

void test_drawing() {
    int ballx = 5;
    int bally = 5;
    int speedx = 5;
    int speedy = 5;
    int scale = 8;

    int width = canvas.width / scale;
    int height = canvas.height / scale;

    for(float x = 0; x < 512; x++) {
		_clearscreen();

        canvas_drawline(4, 4, canvas.width - 4, 4, 0xFFFFFF);
        canvas_drawline(4, 4, 4, canvas.height - 4, 0xFFFFFF);
        canvas_drawline(4, canvas.height - 4, canvas.width - 4, canvas.height - 4, 0xFFFFFF);
        canvas_drawline(canvas.width - 4, 4, canvas.width - 4, canvas.height - 4, 0xFFFFFF);

        canvas_fillrect(ballx * scale, bally * scale, scale, scale, 0x00FFFF);

        _movecursor(2, 1);
        printf("Pong");
        
        canvas_swap();

        ballx += speedx;
        bally += speedy;

        if(ballx <= 0)
            speedx = 1;
        if(ballx >= width)
            speedx = -1;
        if(bally <= 0)
            speedy = 1;
        if(bally >= height)
            speedy = -1;
	}
}

void new_shell() {
	char* input = malloc(sizeof(char) * 64);

	char abc[32];
	
	for(;;) {
		// Show shell prefix
		_puts((char*) SHELL_PROMPT);

		// Clear input string
		memset(input, 0x00, sizeof(char) * 64);

		// Read line from input
		kreadl(input);
		_newline();

		// Parse arguments
		char** args;
		int argc;
		parse_args(input, &argc, args);
        char* cmd = trim(args[0]);

		if(strcmp(cmd, "bootinfo") == 0) {
			print_bootinfo();
		} else if(strcmp(cmd, "testdraw") == 0) {
			test_drawing();
		} else {
			printf("Unknown command: %s\n", cmd);
		}
	}
}

/* Init early stuff */
void init_early() {
	mm_init((uint32_t)_malloc_base);
	serial_init();
}

void init_base() {
	keyboard_init();
	fat_init();
}

void kmain(unsigned long magic, unsigned long addr) {
	init_early();
	{
		/* Check multiboot magic */
  		if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
			// This panic can be seen only in serial port
			// Because canvas isn't initialized yet
      		kprintf("Magic number is 0x%x, but expect 0x%x\n", (unsigned) magic, MULTIBOOT_BOOTLOADER_MAGIC);
      		kpanic("Invalid multiboot magic number!");
    	}

  		mbi = (multiboot_info_t *) addr;

    	canvas_init(
			(uint8_t*)mbi->framebuffer_addr,
			mbi->framebuffer_width,
			mbi->framebuffer_height,
			mbi->framebuffer_bpp,
			mbi->framebuffer_pitch
		);

        _clearscreen();
        _resetline();
	}
	init_base();

	kprintf("Kernel successful loaded\n");

	FILE *file = fopen("/etc/motd", "r");
	kassert(file != NULL, "Motd file not found!");
	char buff[256];
	fread(buff, 256, 1, file);
    printf("\n%s\n", buff);
	fclose(file);

    new_shell();

	// This function not should be finished
    kpanic("kmain is ended");
}