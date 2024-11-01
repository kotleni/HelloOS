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

// Draw a 3D cube by projecting and connecting vertices
// Define the six faces of the cube using vertex indices
int cubeFaces[6][4] = {
    {0, 1, 2, 3},  // Back face
    {4, 5, 6, 7},  // Front face
    {0, 1, 5, 4},  // Bottom face
    {2, 3, 7, 6},  // Top face
    {1, 2, 6, 5},  // Right face
    {0, 3, 7, 4}   // Left face
};

// Function to draw a polygon face of the cube
void drawPolygon(Vec3D *vertices, int *faceIndices, int vertexCount) {
    for (int i = 0; i < vertexCount; i++) {
        int startIdx = faceIndices[i];
        int endIdx = faceIndices[(i + 1) % vertexCount];  // Wrap around to form a closed shape

        int x0, y0, x1, y1;
        projectPoint(projectionDistance, cubeSize, vertices[startIdx], &x0, &y0);
        projectPoint(projectionDistance, cubeSize, vertices[endIdx], &x1, &y1);

        // Calculate the average depth (z) for the edge
        float avgZ = (vertices[startIdx].z + vertices[endIdx].z) / 2.0f;

        // Get shadowed color based on depth
        int shadowedColor = getShadowedColor(avgZ);

        drawLine(x0, y0, x1, y1, shadowedColor);
    }
}

// Helper function to sort vertices by y-coordinate
void sortVerticesByY(int *x, int *y, float *z, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (y[j] > y[j + 1]) {
                int tempX = x[j], tempY = y[j];
                float tempZ = z[j];
                x[j] = x[j + 1]; y[j] = y[j + 1]; z[j] = z[j + 1];
                x[j + 1] = tempX; y[j + 1] = tempY; z[j + 1] = tempZ;
            }
        }
    }
}

int getInterpolatedColor(float z) {
    float minZ = -2.0f;
    float maxZ = 2.0f;
    int brightness = (int)(255 * (maxZ - z) / (maxZ - minZ));
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    return (brightness << 16) | (brightness << 8) | brightness;
}

// Function to fill a polygon face
void drawFilledPolygon(Vec3D *vertices, int *faceIndices, int vertexCount) {
    int x[vertexCount], y[vertexCount];
    float z[vertexCount];
    
    // Project vertices to 2D screen coordinates
    for (int i = 0; i < vertexCount; i++) {
        projectPoint(projectionDistance, cubeSize, vertices[faceIndices[i]], &x[i], &y[i]);
        z[i] = vertices[faceIndices[i]].z;
    }

    // Sort vertices by y-coordinate
    sortVerticesByY(x, y, z, vertexCount);

    // Scanline fill
    for (int j = y[0]; j <= y[vertexCount - 1]; j++) {
        // Find left and right x-coordinates for each scanline
        float startX = x[0], endX = x[0];
        float startZ = z[0], endZ = z[0];
        
        for (int i = 0; i < vertexCount; i++) {
            if (y[i] <= j && y[(i + 1) % vertexCount] >= j) {
                float t = (float)(j - y[i]) / (y[(i + 1) % vertexCount] - y[i]);
                float xi = x[i] + t * (x[(i + 1) % vertexCount] - x[i]);
                float zi = z[i] + t * (z[(i + 1) % vertexCount] - z[i]);
                if (xi < startX) { startX = xi; startZ = zi; }
                if (xi > endX) { endX = xi; endZ = zi; }
            }
        }

        // Interpolate color across the line
        for (int xPos = (int)startX; xPos <= (int)endX; xPos++) {
            float zPos = startZ + (xPos - startX) * (endZ - startZ) / (endX - startX);
            int color = getInterpolatedColor(zPos);
            canvas_drawpixel(xPos, j, color);
        }
    }
}

// Main function to draw the cube by drawing each face as a polygon
void drawCube(float angleX, float angleY) {
    Vec3D transformedVertices[8];

    // Transform each vertex according to the angles
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

    // Draw each face of the cube as a polygon
    for (int i = 0; i < 6; i++) {
        drawFilledPolygon(transformedVertices, cubeFaces[i], 4);  // Each face has 4 vertices
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