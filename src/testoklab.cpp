#include <cstdio>
#include "dither/color_space.h"

struct color_info {
    const char * name;
    float colors[3];
};

color_info colors[] = {
    {"white", {1.0, 1.0, 1.0}},
    {"red", {1.0, 0.0, 0.0}},
    {"green", {0.0, 1.0, 0.0}},
    {"blue", {0.0, 0.0, 1.0}},
    {"cyan", {0.0, 1.0, 1.0}},
    {"magenta", {1.0, 0.0, 1.0}},
    {"yellow", {1.0, 1.0, 0.0}},
    {"black", {0, 0, 0}}
};


int main(int argc, char* argv[]) {
    auto palette_color_space = get_color_space(RGB);
    auto dither_color_space = get_color_space(OKLAB);

    auto palette_to_dither_color_space_converter = get_color_space_converter(palette_color_space, dither_color_space);
    auto dither_to_palette_color_space_converter = get_color_space_converter(dither_color_space, palette_color_space);

    for(unsigned i=0; i<sizeof(colors)/sizeof(color_info); ++i) {
        float output_color[3];
        float compare_color[3];
        palette_to_dither_color_space_converter->convert(*palette_to_dither_color_space_converter, colors[i].colors,  output_color);
        printf("Color: %s { %f, %f, %f }\n", colors[i].name, output_color[0], output_color[1], output_color[2]);
        dither_to_palette_color_space_converter->convert(*dither_to_palette_color_space_converter, output_color,  compare_color);
        printf("Original Color: { %f, %f, %f }\n", compare_color[0], compare_color[1], compare_color[2]);
    }
}
