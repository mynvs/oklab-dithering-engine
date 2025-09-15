#include <memory>

#include "color_space.h"
#include "palette.h"
#include "../path/path.h"

#ifndef DITHER_H
#define DITHER_H 1

#define DITHERVERSION 003 // big.SMALL.TINY format

enum algorithm {errordiffuse, none, ordered};
enum filter {filter_ramp, filter_decimate};

struct dither_settings {
    algorithm stalg;
    filter errorFilter;
    float decimation;
    float decimation_filter_adjust; //only used by error_decimation filter, eventually move into filter setting
    float dither_intensity;
    std::unique_ptr<Path> dither_path;
};

void set_dither_defaults(dither_settings& settings);


void dither(const dither_settings& settings,
            unsigned char* input_image,
            unsigned char* output_image,
            color_space_converter &input_to_dither_colorspace,
            color_space_converter &dither_to_output_colorspace,
            const palette_info& palette,
            int width,
            int height
);


int initdither(const char* color_palette, const char* filen, const char* out, algorithm a, dither_settings &settings,
               std::shared_ptr<color_space> dither_color_space, bool schng
);

#endif
