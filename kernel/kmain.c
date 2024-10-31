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

/* import our font that's in the object file we've created above */
extern char _binary_system_8x16_psf_start;
extern char _binary_system_8x16_psf_end;

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

// FIXME
#define USHRT_MAX 65535
#define PIXEL uint32_t

uint16_t *unicode;

void psf_init()
{
    uint16_t glyph = 0;
    /* cast the address to PSF header struct */
    PSF_font *font = (PSF_font*)&_binary_system_8x16_psf_start;
    /* is there a unicode table? */
    if (font->flags) {
        unicode = NULL;
        return; 
    }

    /* get the offset of the table */
    char *s = (char *)(
    (unsigned char*)&_binary_system_8x16_psf_start +
      font->headersize +
      font->numglyph * font->bytesperglyph
    );
    /* allocate memory for translation table */
    unicode = malloc(USHRT_MAX * 2);
    while(s>_binary_system_8x16_psf_end) {
        uint16_t uc = (uint16_t)((unsigned char *)s[0]);
        if(uc == 0xFF) {
            glyph++;
            s++;
            continue;
        } else if(uc & 128) {
            /* UTF-8 to unicode */
            if((uc & 32) == 0 ) {
                uc = ((s[0] & 0x1F)<<6)+(s[1] & 0x3F);
                s++;
            } else
            if((uc & 16) == 0 ) {
                uc = ((((s[0] & 0xF)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F);
                s+=2;
            } else
            if((uc & 8) == 0 ) {
                uc = ((((((s[0] & 0x7)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F))<<6)+(s[3] & 0x3F);
                s+=3;
            } else
                uc = 0;
        }
        /* save translation */
        unicode[uc] = glyph;
        s++;
    }
}

void putchar(
	unsigned short int c, // WARN: It's not a char, it's unicode char
	int cx, int cy,
	uint32_t fg, uint32_t bg
) {
	PSF_font *font = (PSF_font*)&_binary_system_8x16_psf_start;

	int pixelsize = bpp / 8;
	int scanline = pitch;

	// We need to know how many bytes encode one row
	int bytesperline = (font->width+7) / 8;

	// Unicode translation
	if(unicode != NULL) {
        c = unicode[c];
    }

	// Get the glyph for the character. If there's no
    // glyph for a given character, we'll display the first glyph.
	unsigned char *glyph = (unsigned char*)&_binary_system_8x16_psf_start +
    	font->headersize +
     	(c > 0 && c < font->numglyph ? c : 0) * font->bytesperglyph;

	// Calculate the upper left corner on screen where we want to display.
	// we only do this once, and adjust the offset later. This is faster.
    int offs = (cy * font->height * scanline) +
        ((cx) * (font->width + 1) * pixelsize);

	// Finnaly, displaying pixels
	int x, y, line, mask;

	for(y = 0; y < font->height; y++) { 
		// Save the starting position of the line
        line = offs;
        mask = 1 << (font->width - 1);

		// Display a row
        for(x = 0; x < font->width; x++){
			if(glyph != 0)
            *((PIXEL*)(vga_buffer + line)) = *((unsigned int*)glyph) & mask ? fg : bg;
            /* adjust to the next pixel */
            mask >>= 1;
            line += pixelsize;
        }

		// Adjust to the next line */
        glyph += bytesperline;
        offs  += scanline;
	}
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
	psf_init();
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

	//run_drawlogo();
	char* line = "Penis is big!";
	for(int i = 0; i < 12; i++) {
		if(line[i] == 0x00) break;
		putchar(line[i], i + 1, 1, 0xFFFFFF, 0x000000);
	}

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