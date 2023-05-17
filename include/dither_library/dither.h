#include <memory>

#include "image_buffer.h"
#include "color_space.h"
#include "palette.h"
#include "path.h"

#ifndef DITHER_H
#define DITHER_H 1

enum algorithm {errordiffuse, ordered};
enum filter {filter_ramp, filter_decimate};

struct dither_settings {
    algorithm stalg;
    filter errorFilter;
    float dither_intensity;
    std::unique_ptr<Path> dither_path;
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


int initdither(const char* color_palette, const char* filen, const char* out, algorithm a, dither_settings &settings, 
               std::shared_ptr<color_space> dither_color_space, bool schng);
 
#endif

