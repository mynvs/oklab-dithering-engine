#include "color_space.h"
#include "canvas.h"
#include <limits.h>
#include <float.h>

#ifndef DITHER_H
#define DITHER_H

#define DITHERVERSION 004 // big.SMALL.TINY format

typedef enum {errordiffuse, none, ordered} algorithm;

typedef struct {
    algorithm stalg;
    float dither_intensity;
} dither_settings;

static void set_dither_defaults(dither_settings* settings){
    settings->stalg = errordiffuse;
    settings->dither_intensity = 0.75;
}

static inline int closest_palette_color(const canvasf_t* canvas, const oklab_settings* oklab,
                              float lab_color[3])
{
    int best_index = 0;
    float best_distance = FLT_MAX;

    for (int c = 0; c < canvas->colors; c++) {
        int palette_idx = c * 3;
        float palette_color[3] = {
            canvas->palette[palette_idx],
            canvas->palette[palette_idx+1],
            canvas->palette[palette_idx+2]
        };

        float distance = hcl_oklab_distance(oklab, lab_color, palette_color);
        if (distance < best_distance) {
            best_distance = distance;
            best_index = c;
        }
    }
    return best_index;
}

#endif
