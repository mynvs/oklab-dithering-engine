#include "math.h"
#include "string.h"
#include "limits.h"
#include "float.h"

#ifndef __GNUC__
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "dither/color_space.h"
#include "dither/dither.h"
#include "utils/fileutils.h"

#define SHOW_IMAGE (1)
#define DITHER_BURST (1 << 2)

int initdither(canvas_t* canvas, const char* color_palette, const char* out,
                dither_settings* settings, oklab_settings* oklab)
{
    canvas_set_palette(canvas, color_palette);
    dither(settings, oklab, canvas);

    char* fn = concat_ext(out, "png");
    if(!export_canvas(fn, canvas)){
        printf("failed to save dithered image\n");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    unsigned char flags = 0;
    const char* color_palette = "win256";
    const char* filen = "input.png";
    const char* out = "dither";

    unsigned int plen;
    char** palettes = list_files("palettes", &plen, ".png");
    if(palettes == NULL){
        printf("palettes directory does not exist. you need to create a palette directory to use this software. (try make-palette)\n");
        return 255;
    }
    if(plen == 0){
        printf("theres no palettes in the palette directory.\n");
        return 255;
    }

    canvas_t canvas;
    dither_settings settings;
    oklab_settings color_settings;
    set_dither_defaults(&settings);
    set_oklab_defaults(&color_settings);

    printf("Dither Engine \x1b[90mv\x1b[34;1m%.3d\x1b[0m\n", DITHERVERSION);
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i") && (i+1) < argc)
            filen = argv[++i];
        else if (!strcmp(argv[i], "-o") && (i+1) < argc)
            out = argv[++i];
        else if (!strcmp(argv[i], "-c") && (i+1) < argc) {
            struct stat buffer;
            if (stat(concat_path_ext("palettes", argv[++i], "png"), &buffer) == 0) {
                color_palette = argv[i];
                printf("Using \x1b[90m\"\x1b[0m%s\x1b[90m\"\x1b[0m\x1b[3m color palette\x1b[0m.\n", argv[i]);
            }
            else {
                printf("Palette \x1b[90m\"\x1b[0m%s\x1b[90m\"\x1b[0m\x1b[31m doesn't exist\x1b[0m.\n", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-a") && (i+1) < argc) {
            const char* dt = argv[++i];

            if (strcmp(dt, "none") == 0)
                settings.stalg = none;
            else if (strcmp(dt, "errdiff") == 0)
                settings.stalg = errordiffuse;
            else if (strcmp(dt, "bayer") == 0) {
                printf("Using bayer matrix\n");
                settings.stalg = ordered;
            }
            else {
                printf("Invalid algorithm selection.");
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-p") && (i+1) < argc) {
            const char* dt = argv[++i];
            if (strcmp(dt, "alien") == 0) {
                color_settings.l_offset = -0.3f;
                color_settings.l_scale = 2.5f;
                color_settings.hue_cos_scale = cos(.825f);
                color_settings.hue_sin_scale = sin(.825f);
            }
            if (strcmp(dt, "abomination") == 0) {
                color_settings.l_offset = -1.2f;
                color_settings.l_scale = 3.5f;
                color_settings.hue_cos_scale = cos(1.825f);
                color_settings.hue_sin_scale = sin(.88f);
            }
            if (strcmp(dt, "cyberblue") == 0) {
                color_settings.l_offset = 1.12f;
                color_settings.l_scale = 1.8f;
                color_settings.hue_cos_scale = cos(0.825f);
                color_settings.hue_sin_scale = sin(1.28f);
            }
            else if (strcmp(dt, "invert") == 0) {
                color_settings.l_scale = -1.0f;
                color_settings.hue_cos_scale = -1.0;
                color_settings.hue_sin_scale = 0.0;
            }
        }
        else if (!strcmp(argv[i], "--dintensity") && (i+1) < argc) {
            settings.dither_intensity = atof(argv[++i]);
        }
        else if (!strcmp(argv[i], "--show")) {
            flags |= SHOW_IMAGE;
        }
        else if (!strcmp(argv[i], "--allp")) {
            flags |= DITHER_BURST;
        }
        else if (!strcmp(argv[i], "--loffset")) {
            float luminance_offset = FLT_MAX;
            if(i+1 < argc) {
                luminance_offset = atof(argv[++i]);
            }

            if(!isinf(luminance_offset) || !isnan(luminance_offset)) {
                color_settings.l_offset += luminance_offset;
            } else {
                printf("--luminance_offset reguires a floating point argument.\n");
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--lscale")) {
            float luminance_scale = FLT_MAX;
            if(i+1 < argc) {
                luminance_scale = atof(argv[++i]);
            }
            if(!isinf(luminance_scale)) {
                color_settings.l_scale *= luminance_scale;
            } else {
                printf("--luminance_scale reguires a floating point argument.\n");
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--distancetune")) {
            float hue_distance_weight = FLT_MAX;
            float chroma_distance_weight = FLT_MAX;
            float luminance_distance_weight = FLT_MAX;
            if(i+3 < argc) {
                hue_distance_weight = atof(argv[++i]);
                chroma_distance_weight = atof(argv[++i]);
                luminance_distance_weight = atof(argv[++i]);
            }
            if(isinf(hue_distance_weight) || isinf(hue_distance_weight) || isinf(hue_distance_weight)) {
                printf("--distancetune reguires 3 decimal numbers specified.\n");
                return 1;
            } else {
                color_settings.hue_distance_weight = hue_distance_weight;
                color_settings.chroma_distance_weight = chroma_distance_weight;
                color_settings.luminance_distance_weight = luminance_distance_weight;
            }
        }
    }

    if(!canvas_create(&canvas, filen)){
        return 1;
    }

    if (!(flags & DITHER_BURST)) {
        if (initdither(&canvas, concat_path_ext("palettes", color_palette, "png"), out, &settings, &color_settings)) {
            printf("Dither \x1b[31mfailed\x1b[0m.\n");
            return 2;
        }
        if ((flags & SHOW_IMAGE)) {
            system(concat_ext(out, "png"));
        }
    }
    else {
        mkdir("burstdither", 0775);
        printf("%s%d%s", "\x1b[90m", plen, "\x1b[0m palettes active\n\n");
        for (unsigned int i = 0; i < plen; i++) {
            printf("%s%s%s\n",
                "Dithering using palette \x1b[90m\"\x1b[0m",
                filename_without_extension(palettes[i]),
                "\x1b[90m\"\x1b[0m"
            );
            if (initdither(&canvas, palettes[i], concat_paths("burstdither", filename_without_extension(palettes[i])), &settings, &color_settings)) {
                printf("Dither \x1b[31mfailed\x1b[0m.\n");
                return 2;
            }
        }
    }
    canvas_clear(&canvas);
    printf("Complete!\n");
    free(palettes);
    return 0;
}
