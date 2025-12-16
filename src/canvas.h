/*

    Centralized way of doing stuff with the images with everything in one struct.

*/

#include "stdio.h"
#include "color_space.h"

#ifndef CANVAS_H
#define CANVAS_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"

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

static int canvas_create(canvas_t* canvas, const char* inputfn) {
    int c;
    if((canvas->input = stbi_load(inputfn, &canvas->width, &canvas->height, &canvas->channels, 4)) == NULL){
        printf("%s\n", "Couldn't load input buffer into canvas.");
        canvas->width = canvas->height = canvas->channels = canvas->colors = 0;
        return 0;
    } // allow transparency

    if((canvas->output = (unsigned char*)malloc(canvas->width * canvas->height * 4)) == NULL){
        printf("%s\n", "canvas output buffer pre-allocation error");
        canvas->width = canvas->height = canvas->channels = canvas->colors = 0;
        return 0;
    } // PREALLOCATE!!!

    canvas->palette = NULL; // init with null
    canvas->channels = 4; // force alpha
    return 1;
}

static void canvas_set_palette(canvas_t* canvas, const char* palettefn) {
    if(canvas->palette != NULL){
        free(canvas->palette);
        canvas->palette = NULL;
        canvas->colors = 0;
    }
    int cw, ch, c; // channels unused
    if((canvas->palette = stbi_load(palettefn, &cw, &ch, &c, 3)) == NULL){
        printf("%s\n", "Couldn't load color palette buffer into canvas.");
        canvas->width = canvas->height = canvas->channels = canvas->colors = 0;
    } // desired channels is 3 because palettes never house mapped alpha values.
    canvas->colors = cw*ch; // flatten to 1d for maximum memory seek throughput
}

static int export_canvas(const char* outputfn, canvas_t* inp) {
    return stbi_write_png(outputfn, inp->width, inp->height, inp->channels, inp->output, inp->width * inp->channels);
}

// convert to be using oklab color space for better color usage
static canvasf_t oklab_canvas(canvas_t* inp) {
    canvasf_t canvas = {inp->width, inp->height, inp->channels, inp->colors,
        (float*)malloc((inp->width*inp->height*inp->channels) * sizeof(float)),
        (float*)malloc((inp->colors * 3) * sizeof(float)),
        (float*)malloc((inp->width*inp->height*inp->channels) * sizeof(float))
    };
    if(!canvas.input || !canvas.palette || !canvas.output){
        printf("%s\n", "canvas buffer allocation error (oklab conversion)");
        canvas.width = canvas.height = canvas.channels = canvas.colors = 0;
        return canvas;
    }

    for (int i = 0; i < inp->width*inp->height; i++) {
        int p = i * inp->channels;
        float source_color[3];
        float target_color[3];
        byte_triple_to_floats(&inp->input[p], source_color);
        srgb_oklab(source_color, target_color);
        canvas.input[p] = target_color[0];
        canvas.input[p+1] = target_color[1];
        canvas.input[p+2] = target_color[2];
        canvas.input[p+3] = inp->input[p+3] * INT8_REC; // preserve transparency!
    }
    for (int i = 0; i < inp->colors; i++) { // palette conversion for 3d distance calculation thingie
        int p = i * 3;
        float source_color[3];
        float target_color[3];
        byte_triple_to_floats(&inp->palette[p], source_color);
        srgb_oklab(source_color, target_color);
        canvas.palette[p] = target_color[0];
        canvas.palette[p+1] = target_color[1];
        canvas.palette[p+2] = target_color[2];
    }
    return canvas;
}

// convert back to srgb8
static canvas_t srgb_canvas(canvasf_t* inp) {
    canvas_t canvas = {inp->width, inp->height, inp->channels, inp->colors,
        (unsigned char*)malloc(inp->width*inp->height*inp->channels),
        (unsigned char*)malloc(inp->colors * 3),
        (unsigned char*)malloc(inp->width*inp->height*inp->channels)
    };
    if(!canvas.input || !canvas.palette || !canvas.output){
        printf("%s\n", "canvas buffer allocation error (oklab -> srgb conversion)");
        canvas.width = canvas.height = canvas.channels = canvas.colors = 0;
        return canvas;
    }

    for (int i = 0; i < inp->width*inp->height; i++) {
        int p = i * inp->channels;
        float target_color[3];
        oklab_srgb(&inp->input[p], target_color);
        float_triple_to_bytes(target_color, &canvas.input[p]);
        canvas.input[p+3] = cclamp(inp->input[p+3] * 255.0f); // preserve transparency!
    }
    for (int i = 0; i < inp->colors; i++) { // palette conversion for 3d distance calculation thingie
        int p = i * 3;
        float target_color[3];
        oklab_srgb(&inp->palette[p], target_color);
        float_triple_to_bytes(target_color, &canvas.palette[p]);
    }
    for (int i = 0; i < inp->width*inp->height; i++) { // kljsakljfhyklasjhyfkuxvzjkkj
        int p = i * inp->channels;
        float target_color[3];
        oklab_srgb(&inp->output[p], target_color);
        float_triple_to_bytes(target_color, &canvas.output[p]);
        canvas.output[p+3] = cclamp(inp->output[p+3] * 255.0f); // preserve transparency!
    }
    return canvas;
}

static void canvasf_clear(canvasf_t* cnv) {
    free(cnv->input);
    free(cnv->output);
    free(cnv->palette);
}

static void canvas_clear(canvas_t* cnv) {
    free(cnv->input);
    free(cnv->output);
    free(cnv->palette);
}

#endif
