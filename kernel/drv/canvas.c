#include <drv/canvas.h>

canvas_t canvas;

void canvas_init(uint8_t* framebuffer, int width, int height, int bpp, int pitch) {
    canvas.framebuffer = framebuffer;
    int pixelwidth = bpp / 8;
    canvas.backbuffer = (uint8_t*)malloc(sizeof(uint8_t) * (width*pixelwidth+height*pitch));
    canvas.width = width;
    canvas.height = height;
    canvas.bpp = bpp;
    canvas.pitch = pitch;

    psf_load();
}

uint8_t* canvas_getbuffer() {
    return canvas.backbuffer;
}

void canvas_swap() {
    int pixelwidth = canvas.bpp / 8;
    int len = (canvas.width*pixelwidth+canvas.height*canvas.pitch);
    memcpy(canvas.framebuffer, canvas_getbuffer(), len);
}

void canvas_drawpixel(int x,int y, int color) {
	int pixelwidth = canvas.bpp / 8;

    unsigned where = x*pixelwidth + y*canvas.pitch;
    canvas_getbuffer()[where] = color & 255;              // BLUE
    canvas_getbuffer()[where + 1] = (color >> 8) & 255;   // GREEN
    canvas_getbuffer()[where + 2] = (color >> 16) & 255;  // RED
}

void canvas_fillrect(int x, int y, int w, int h, int color) {
    unsigned char *where = canvas_getbuffer();
    int i, j;

    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            canvas_drawpixel(x + j, y + i, color);
        }
        where += canvas.pitch;
    }
}

void canvas_drawchar(
	unsigned short int c, // WARN: It's not a char, it's unicode char
	int cx, int cy,
	uint32_t fg, uint32_t bg
) {
	PSF_font *font = (PSF_font*)&_binary_system_8x16_psf_start;

	int pixelsize = canvas.bpp / 8;
	int scanline = canvas.pitch;

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
            *((PIXEL*)(canvas_getbuffer() + line)) = *((unsigned int*)glyph) & mask ? fg : bg;
            /* adjust to the next pixel */
            mask >>= 1;
            line += pixelsize;
        }

		// Adjust to the next line */
        glyph += bytesperline;
        offs  += scanline;
	}
}

void canvas_drawline(int x0, int y0, int x1, int y1, int color) {
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

void canvas_projectpoint(float projectionDistance, float scale, Vec3D point, int *x, int *y) {
    float factor = projectionDistance / (projectionDistance + point.z);
    *x = (int)(point.x * factor * scale + canvas.width / 2);
    *y = (int)(point.y * factor * scale + canvas.height / 2);
}