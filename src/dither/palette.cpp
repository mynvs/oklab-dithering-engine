#include <cstring>
#include "color_space.h"
#include "palette.h"
#include "imghandle.h"

void load_palette(const std::string& palette_filename, const color_space_converter &converter, palette_info& palette) {

    image_info file_info;
    unsigned char* palette_buffer = load_image(palette_filename, file_info);

    int num_colors = file_info.width * file_info.height;

    palette.num_colors = num_colors;
    palette.c0 = new float[palette.num_colors];
    palette.c1 = new float[palette.num_colors];
    palette.c2 = new float[palette.num_colors];

    memset(palette.c0, 0, sizeof(float) * num_colors);
    memset(palette.c1, 0, sizeof(float) * num_colors);
    memset(palette.c2, 0, sizeof(float) * num_colors);

    for (int i = 0; i < num_colors; i++) {
        float input_color[3]{};
        float output_color[3];

        byte_triple_to_floats(&palette_buffer[i * 3], input_color);
        converter.convert(converter, input_color, output_color);

        palette.c0[i] = output_color[0];
        palette.c1[i] = output_color[1];
        palette.c2[i] = output_color[2];
    }
    delete[] palette_buffer;
}
