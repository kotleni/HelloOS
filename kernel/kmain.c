/* HelloOS */
#include <kernel.h>
#include <stdio2.h>

extern uint8_t _kernel_base[];
extern uint8_t _malloc_base[];
multiboot_info_t *mbi;

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

		if(strcmp(cmd, "info") == 0) {
			
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
	
	kprintf("Kernel base addr: 0x%x\n", _kernel_base);
	kprintf("Kernel end addr: 0x%x\n", _malloc_base);
	kprintf("Available RAM: %dmb\n", mbi->mem_upper / 1024);

	FILE *file = fopen("/ETC/MOTD", "r");
	kassert(file != NULL, "Motd file not found!");
	char buff[256];
	fread(buff, 256, 1, file);
	display_putch('\n');
	display_puts(buff);
	display_putch('\n');
	fclose(file);

    new_shell();

	// This function not should be finished
    kpanic("kmain is ended");
}