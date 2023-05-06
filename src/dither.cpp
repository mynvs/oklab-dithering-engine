#include <limits>
#include <stdio.h>
#include "dither.h"

#define ERROR_LENGTH 7

void set_dither_defaults(dither_settings& settings) {
    //-ALGORITHM TYPE-//
    settings.stalg = errordiffuse;
    settings.dither_intensity = 0.75;
    // --------------------------- //
}

// float[3], float[3] -> float; euclidean distance between two 3d points
static float hypot(float a[3], float b[3]) {
    const float x = b[0] - a[0];
    const float y = b[1] - a[1];
    const float z = b[2] - a[2];
    return x * x + y * y + z * z;
}

void dither(const dither_settings& settings,
    image_buffer_t input_image,
    image_buffer_t output_image,
    color_space_converter &input_to_dither_colorspace,
    color_space_converter &dither_to_output_colorspace,
    const palette_info& palette,
    int width,
    int height) {

    int num_pixels = width * height;
    float infinity = std::numeric_limits<float>::infinity();

    unsigned input_image_offset = 0;
    unsigned output_image_offset = 0;
    auto palette_channel0 = palette.channel0.get();
    auto palette_channel1 = palette.channel1.get();
    auto palette_channel2 = palette.channel2.get();
    int num_colors = palette.num_colors;

    if (settings.stalg == errordiffuse) {
        const float negative_dither_intensity = -settings.dither_intensity;
        float weights[ERROR_LENGTH]{};
        for (int i = 0; i < ERROR_LENGTH; i++) {
            weights[i] = 1.0 / (i + 1) * negative_dither_intensity;
        }

        float errors_0[ERROR_LENGTH];
        float errors_1[ERROR_LENGTH];
        float errors_2[ERROR_LENGTH];
        for (int i = 0; i < ERROR_LENGTH; i++) {
            errors_0[i] = 0.0;
            errors_1[i] = 0.0;
            errors_2[i] = 0.0;
        }

        for (int i = 0; i < num_pixels; i++) {
            float source_color[3];
            float target_color[3];
            byte_triple_to_floats(&input_image[input_image_offset], source_color); 
            input_image_offset += 3;
            input_to_dither_colorspace.convert(input_to_dither_colorspace, source_color, target_color);
           
            float old_target_color[3]{};
            old_target_color[0] = target_color[0];
            old_target_color[1] = target_color[1];
            old_target_color[2] = target_color[2];
            for (int m = 0; m < ERROR_LENGTH; m++) {
                target_color[0] += errors_0[m] * weights[m];
                target_color[1] += errors_1[m] * weights[m];
                target_color[2] += errors_2[m] * weights[m];
            }

            int amin_delta = 0;
            float best_delta = infinity;
            for (int d = 0; d < num_colors; d++) {

                float palette_color[3]{};
                palette_color[0] = palette_channel0[d];
                palette_color[1] = palette_channel1[d];
                palette_color[2] = palette_channel2[d];

                float delta = hypot(target_color, palette_color);
                if (delta < best_delta) {
                    amin_delta = d;
                    best_delta = delta;
                }
            }

            float final_color[3]{};
            final_color[0] = palette_channel0[amin_delta];
            final_color[1] = palette_channel1[amin_delta];
            final_color[2] = palette_channel2[amin_delta];

            float output_color[3];
            dither_to_output_colorspace.convert(dither_to_output_colorspace, final_color, output_color);
            float_triple_to_bytes(output_color, &output_image[output_image_offset]);
            output_image_offset += 3;

            for (int m = ERROR_LENGTH - 1; m > 0; m--) {
                errors_0[m] = errors_0[m - 1];
                errors_1[m] = errors_1[m - 1];
                errors_2[m] = errors_2[m - 1];
            }
            errors_0[0] = final_color[0] - old_target_color[0];
            errors_1[0] = final_color[1] - old_target_color[1];
            errors_2[0] = final_color[2] - old_target_color[2];
        }
    }

    else if (settings.stalg == ordered) {
#if 0 
        //FIXME   
        const int matrix_size = 4;
        const int matrix[matrix_size][matrix_size] = { {0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5} };

        // TODO: Ordered algorithm fix

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float lab[3];
                srgb_oklab(&input_image[(y * width + x) * 3], lab);

                // Find the closest color in the palette to the pixel color
                float min_dist = std::numeric_limits<float>::infinity();
                int min_idx = -1;
                for (int i = 0; i < num_colors; i++) {
                    float L_diff = lab[0] - palette_L[i];
                    float a_diff = lab[1] - palette_a[i];
                    float b_diff = lab[2] - palette_b[i];
                    float dist = std::sqrt(L_diff * L_diff + a_diff * a_diff + b_diff * b_diff);
                    if (dist < min_dist) {
                        min_dist = dist;
                        min_idx = i;
                    }
                }

                // Dither using the closest color in the palette
                int ix = x % matrix_size;
                int iy = y % matrix_size;
                float threshold = (matrix[iy][ix] + 0.5) / (matrix_size * matrix_size);
                float l = lab[0];
                if (l <= threshold) {
                    l = palette_L[min_idx];
                }
                else {
                    l = palette_L[min_idx] + 1.0 / 65535.0; // Add a small amount to avoid quantization artifacts
                }
                lab[0] = l;
                lab[1] = palette_a[min_idx] + settings.dither_intensity * (lab[1] - palette_a[min_idx]);
                lab[2] = palette_b[min_idx] + settings.dither_intensity * (lab[2] - palette_b[min_idx]);

                oklab_srgb(lab, &output_image[(y * width + x) * 3]);
            }
        }
#endif
    }
}