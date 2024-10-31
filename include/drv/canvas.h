#ifndef H_VIDEO
#define H_VIDEO

#include <types.h>
#include <misc/psf.h>

#define PIXEL uint32_t

struct _canvas {
    uint8_t* framebuffer;
    int width, height;
    int bpp, pitch;
};
typedef struct _canvas canvas_t;

extern canvas_t canvas;

void canvas_init(uint8_t* framebuffer, int width, int height, int bpp, int pitch);
void canvas_drawpixel(int x,int y, int color);
void canvas_fillrect(unsigned char r, unsigned char g, unsigned char b, unsigned char w, unsigned char h);
void canvas_drawchar(unsigned short int c, int cx, int cy,uint32_t fg, uint32_t bg);

#endif