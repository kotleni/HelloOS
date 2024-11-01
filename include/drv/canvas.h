#ifndef H_VIDEO
#define H_VIDEO

#include <types.h>
#include <misc/psf.h>
#include <math.h>
#include <misc/malloc.h>

#define PIXEL uint32_t

struct _canvas {
    uint8_t* framebuffer;
    // Temporary buffer for frame
    uint8_t* backbuffer;
    int width, height;
    int bpp, pitch;
};
typedef struct _canvas canvas_t;

typedef struct {
    float x, y, z;
} Vec2D;

typedef struct {
    float x, y, z;
} Vec3D;

typedef struct {
    int start, end;
} Edge;

extern canvas_t canvas;

void canvas_init(uint8_t* framebuffer, int width, int height, int bpp, int pitch);
uint8_t *canvas_getbuffer();
/* Swap virtual frame buffer to real if needed */
void canvas_swap();
void canvas_drawpixel(int x,int y, int color);
void canvas_fillrect(int x, int y, int w, int h, int color);
void canvas_drawchar(unsigned short int c, int cx, int cy,uint32_t fg, uint32_t bg);

// Project 3D points to 2D screen coordinates
void canvas_projectpoint(float projectionDistance, float scale, Vec3D point, int *x, int *y);

// Draw line function using Bresenham's algorithm
void canvas_drawline(int x0, int y0, int x1, int y1, int color);

#endif