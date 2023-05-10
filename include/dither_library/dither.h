/*
#include <limits>
#include <math.h>
#include <fstream>
#include <cmath>
#include <cstring>
#include <memory>
#include <sstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#define DEL "del"
#else
#include <unistd.h>
#define DEL "rm"
#endif
#include <chrono>

#define TAU 6.283185307179586

#define ERROR_LENGTH 7
*/

#include "image_buffer.h"
#include "color_space.h"
#include "palette.h"

#ifndef DITHER_H
#define DITHER_H 1

enum algorithm {errordiffuse, ordered};

struct dither_settings {
    algorithm stalg;
    float l_offset;
    float l_scale; // -inf to +inf
    float hue_phase;
    float hue_scale;
    float a_offset;
    float b_offset;
    float dither_intensity;
};

void set_dither_defaults(dither_settings& settings);


void dither(const dither_settings& settings,
            image_buffer_t input_image,
            image_buffer_t output_image,
            color_space_converter &input_to_dither_colorspace,
            color_space_converter &dither_to_output_colorspace,
            const palette_info& palette,
            int width,
            int height);

#endif

