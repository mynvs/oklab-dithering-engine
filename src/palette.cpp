#include <cstring>
#include "color_space.h"
#include "palette.h"
#include "ppm.h"

void load_palette(const std::string& palette_ppm_filename, const color_space_converter &converter, palette_info& palette) {

    ppm_file_info file_info;
    image_buffer_t palette_buffer = load_ppm_image(palette_ppm_filename, file_info);

    int num_colors = file_info.width * file_info.height;

    palette.num_colors = num_colors;
    palette.channel0 = std::shared_ptr<float[]>(new float[palette.num_colors]);
    palette.channel1 = std::shared_ptr<float[]>(new float[palette.num_colors]);
    palette.channel2 = std::shared_ptr<float[]>(new float[palette.num_colors]);

    memset(palette.channel0.get(), 0, sizeof(float) * num_colors);
    memset(palette.channel1.get(), 0, sizeof(float) * num_colors);
    memset(palette.channel2.get(), 0, sizeof(float) * num_colors);

    unsigned palette_buffer_offset = 0;
    for (int i = 0; i < num_colors; i++) {
        float input_color[3]{};
        float output_color[3];
        byte_triple_to_floats(&palette_buffer[palette_buffer_offset], input_color);
        palette_buffer_offset+=3;
        
        converter.convert(converter, input_color, output_color);

        palette.channel0[i] = output_color[0];
        palette.channel1[i] = output_color[1];
        palette.channel2[i] = output_color[2];
    }
}

