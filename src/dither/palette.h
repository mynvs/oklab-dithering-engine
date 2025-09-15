#include <memory>
#include <iostream>

#include "color_space.h"

#ifndef PALETTE_H
#define PALETTE_H 1

struct palette_info {
    int num_colors;
    float* c0, *c1, *c2;
};

void load_palette(const std::string& palette_ppm_filename, const color_space_converter &converter, palette_info& palette);

#endif
