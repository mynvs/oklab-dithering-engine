#include "color_space.h"
#include "canvas.h"

#ifndef DITHER_H
#define DITHER_H 1

#define DITHERVERSION 004 // big.SMALL.TINY format

typedef enum {errordiffuse, none, ordered} algorithm;

typedef struct {
    algorithm stalg;
    float dither_intensity;
} dither_settings;

void set_dither_defaults(dither_settings* settings);

void dither(const dither_settings* settings, const oklab_settings* oklab, canvas_t* canvas);

int initdither(canvas_t* canvas, const char* color_palette, const char* out,
                dither_settings* settings, oklab_settings* oklab);

#endif
