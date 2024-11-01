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

typedef struct {
    float x, y, z;
} Vec3D;

typedef struct {
    int start, end;
} Edge;

// Define the cube vertices (scaled and centered)
Vec3D cubeVertices[8] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},  // Back face
    {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}   // Front face
};

// Define the cube edges by specifying start and end vertices
Edge cubeEdges[12] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Back face edges
    {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Front face edges
    {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting edges between faces
};

// Approximate sine and cosine using Taylor series for small angles
float sin(float angle) {
    float angle3 = angle * angle * angle;
    return angle - (angle3 / 6.0f);  // Approximation of sine (angle - angle^3 / 3!)
}

float cos(float angle) {
    float angle2 = angle * angle;
    return 1.0f - (angle2 / 2.0f);  // Approximation of cosine (1 - angle^2 / 2!)
}

// Parameters
float projectionDistance = 2.5;  // Distance to projection plane
float cubeSize = 50;             // Scaling factor for the cube size

int getShadowedColor(float z) {
    // Set the depth range for color adjustments (customize as needed)
    float minZ = -2.0f;  // Farther
    float maxZ = 2.0f;   // Closer

    // Calculate brightness from 0 to 255 based on z-depth
    int brightness = (int)(255 * (maxZ - z) / (maxZ - minZ));
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;

    // Combine brightness into an RGB color (white with varying intensity)
    return (brightness << 16) | (brightness << 8) | brightness;
}

// Project 3D points to 2D screen coordinates
void projectPoint(Vec3D point, int *x, int *y) {
    float factor = projectionDistance / (projectionDistance + point.z);
    *x = (int)(point.x * factor * cubeSize + canvas.width / 2);
    *y = (int)(point.y * factor * cubeSize + canvas.height / 2);
}

// Draw line function using Bresenham's algorithm
void drawLine(int x0, int y0, int x1, int y1, int color) {
    int dx = x1 > x0 ? (x1 - x0) : (x0 - x1);
    int dy = y1 > y0 ? (y1 - y0) : (y0 - y1);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        canvas_drawpixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

// Draw a 3D cube by projecting and connecting vertices
void drawCube(float angleX, float angleY) {
    Vec3D transformedVertices[8];

    for (int i = 0; i < 8; i++) {
        // Rotate around X axis
        float x = cubeVertices[i].x;
        float y = cubeVertices[i].y * cos(angleX) - cubeVertices[i].z * sin(angleX);
        float z = cubeVertices[i].y * sin(angleX) + cubeVertices[i].z * cos(angleX);

        // Rotate around Y axis
        float x2 = x * cos(angleY) + z * sin(angleY);
        float z2 = -x * sin(angleY) + z * cos(angleY);

        // Store transformed vertex
        transformedVertices[i].x = x2;
        transformedVertices[i].y = y;
        transformedVertices[i].z = z2;
    }

    // Draw edges between transformed vertices
    for (int i = 0; i < 12; i++) {
        int startIdx = cubeEdges[i].start;
        int endIdx = cubeEdges[i].end;

        int x0, y0, x1, y1;
        projectPoint(transformedVertices[startIdx], &x0, &y0);
        projectPoint(transformedVertices[endIdx], &x1, &y1);

		/// Calculate the average depth (z) for the edge
        float avgZ = (transformedVertices[startIdx].z + transformedVertices[endIdx].z) / 2.0f;

        // Get shadowed color based on depth
        int shadowedColor = getShadowedColor(avgZ);

        drawLine(x0, y0, x1, y1, shadowedColor);  // Cyan color
    }
}

void test_drawcube() {
	float max = 1.8f;
    for(float x = -max; x < max; x += 0.001) {
		_clearscreen();
		drawCube(x, 0.5);
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
		} else if(strcmp(cmd, "testcube") == 0) {
			test_drawcube();
		} else {
			kprintf("Unknown command %s!\n", args[0]);
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
	}
	init_base();

	kprintf("Started HelloOS v0.6\n");

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