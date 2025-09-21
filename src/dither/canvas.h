/*

    Centralized way of doing stuff with the images with everything in one struct.

*/

#include "stdio.h"
#include "color_space.h"

#ifndef CANVAS_H
#define CANVAS_H

typedef struct {
    int width, height, channels, colors; // colors: number of colors in palette
    unsigned char* input;
    unsigned char* palette; // RGB8 for maximum compatibility and speed for other algorithms (alpha not supported)
    unsigned char* output; // output to save the results
} canvas_t;

typedef struct {
    int width, height, channels, colors; // colors: number of colors in palette
    float* input;
    float* palette; // FRGB (alpha not supported)
    float* output; // should be used for writting stuff into before converting back to RGB8
} canvasf_t;

int canvas_create(canvas_t* canvas, const char* inputfn);
void canvas_set_palette(canvas_t* canvas, const char* palettefn);
int export_canvas(const char* outputfn, canvas_t* inp);

// convert to be using oklab color space for better color usage
canvasf_t oklab_canvas(canvas_t* inp);
// convert back to srgb8
canvas_t srgb_canvas(canvasf_t* inp);

void canvasf_clear(canvasf_t* cnv);
void canvas_clear(canvas_t* cnv);

#endif
