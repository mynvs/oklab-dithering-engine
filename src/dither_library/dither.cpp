#include <cmath>
#include <limits>
#include <vector>
#include "dither.h"
#include "design_filter_1d.h"
#include "fir_1d.h"
#include "path_choices.h"
#include "path_iterator.h"

#define ERROR_LENGTH 25 //From 7

void set_dither_defaults(dither_settings& settings) {
    //-ALGORITHM TYPE-//
    settings.stalg = errordiffuse;
    settings.errorFilter = filter_ramp;
    settings.decimation = 1.0;
    settings.decimation_filter_adjust = 0.0;
    settings.dither_intensity = 0.75;
    settings.dither_path = get_path("sequential");
    // --------------------------- //
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

    std::shared_ptr<FloatRectangle> channel0_input = std::make_shared<FloatRectangle>(width, height);
    std::shared_ptr<FloatRectangle> channel1_input = std::make_shared<FloatRectangle>(width, height);
    std::shared_ptr<FloatRectangle> channel2_input = std::make_shared<FloatRectangle>(width, height);
    std::shared_ptr<FloatRectangle> channel0_output = std::make_shared<FloatRectangle>(width, height);
    std::shared_ptr<FloatRectangle> channel1_output = std::make_shared<FloatRectangle>(width, height);
    std::shared_ptr<FloatRectangle> channel2_output = std::make_shared<FloatRectangle>(width, height);

    //Do colorspace conversion and setup float buffers and path walking
    for (int i = 0; i < num_pixels; i++) {
        float source_color[3];
        float target_color[3];
        byte_triple_to_floats(&input_image[input_image_offset], source_color);
        input_image_offset += 3;
        input_to_dither_colorspace.convert(input_to_dither_colorspace, source_color, target_color);

        channel0_input->set((unsigned)i, target_color[0]);
        channel1_input->set((unsigned)i, target_color[1]);
        channel2_input->set((unsigned)i, target_color[2]);
    }
    auto channel0_input_iterator = PathIterator(channel0_input, settings.dither_path->clone());
    auto channel1_input_iterator = PathIterator(channel1_input, settings.dither_path->clone());
    auto channel2_input_iterator = PathIterator(channel2_input, settings.dither_path->clone());
    auto channel0_output_iterator = PathIterator(channel0_output, settings.dither_path->clone());
    auto channel1_output_iterator = PathIterator(channel1_output, settings.dither_path->clone());
    auto channel2_output_iterator = PathIterator(channel2_output, settings.dither_path->clone());

    if(settings.decimation != 1.0) {
        float filter_color[3];
        unsigned int filterDelay = 0;
        auto data = designDecimationFilter(settings.decimation, filterDelay);
        printf("filter delay %u\n", filterDelay);

        std::vector<float> weights;
        for(unsigned i=0; i< data->size(); ++i) {
            weights.push_back((*data)[i]);
        }
        FirFilter1d<float> filter0 = FirFilter1d<float>(weights);
        FirFilter1d<float> filter1 = FirFilter1d<float>(weights);
        FirFilter1d<float> filter2 = FirFilter1d<float>(weights);
        for (unsigned int i = 0; i < channel0_input_iterator.getRectangle()->getPixels(); i++) {

            filter_color[0] = filter0.filter(*channel0_input_iterator++);
            filter_color[1] = filter1.filter(*channel1_input_iterator++);
            filter_color[2] = filter2.filter(*channel2_input_iterator++);
            if(i>=filterDelay) {
                (*channel0_output_iterator++) = filter_color[0];
                (*channel1_output_iterator++) = filter_color[1];
                (*channel2_output_iterator++) = filter_color[2];
            }
        }
        for(unsigned i =0; i<filterDelay; ++i) {
            filter_color[0] = filter0.filter(0.0);
            filter_color[1] = filter1.filter(0.0);
            filter_color[2] = filter2.filter(0.0);
            (*channel0_output_iterator++) = filter_color[0];
            (*channel1_output_iterator++) = filter_color[1];
            (*channel2_output_iterator++) = filter_color[2];
        }
        auto rectangle0 = channel0_output_iterator.getRectangle();
        auto rectangle1 = channel1_output_iterator.getRectangle();
        auto rectangle2 = channel2_output_iterator.getRectangle();
        rectangle0->depad();
        rectangle1->depad();
        rectangle2->depad();
        channel0_input_iterator = PathIterator(rectangle0, settings.dither_path->clone());
        channel1_input_iterator = PathIterator(rectangle1, settings.dither_path->clone());
        channel2_input_iterator = PathIterator(rectangle2, settings.dither_path->clone());
        channel0_output = std::make_shared<FloatRectangle>(width, height);
        channel1_output = std::make_shared<FloatRectangle>(width, height);
        channel2_output = std::make_shared<FloatRectangle>(width, height);
        channel0_output_iterator = PathIterator(channel0_output, settings.dither_path->clone());
        channel1_output_iterator = PathIterator(channel1_output, settings.dither_path->clone());
        channel2_output_iterator = PathIterator(channel2_output, settings.dither_path->clone());
    }

    if (settings.stalg == errordiffuse) {
        const float negative_dither_intensity = -settings.dither_intensity;

        std::vector<float> weights;
        if (settings.errorFilter == filter_ramp) {
            for(int i=0; i<ERROR_LENGTH; ++i) {
                weights.insert(weights.begin(), 1.0 / (i+1) * negative_dither_intensity);
            }
        } else if (settings.errorFilter == filter_decimate) {
            float bitCount = std::log(num_colors)/std::log(2.0);
            float decimationRatio = 24.0/bitCount;

            decimationRatio -= (decimationRatio - 1.0)*settings.decimation_filter_adjust;
            printf("Using decimation ratio: %f\n", decimationRatio);

            auto data = designHalfDecimationFilter(decimationRatio);
            for(unsigned i=0; i< data->size(); ++i) {
                weights.push_back((*data)[i] * negative_dither_intensity);
            }
        }
        FirFilter1d<float> filter0 = FirFilter1d<float>(weights);
        FirFilter1d<float> filter1 = FirFilter1d<float>(weights);
        FirFilter1d<float> filter2 = FirFilter1d<float>(weights);
        float filter0_error=0.0f;
        float filter1_error=0.0f;
        float filter2_error=0.0f;

        auto space = input_to_dither_colorspace.destination;

        //getPixels());
        //Perform dither
        for (unsigned i = 0; i < channel0_input_iterator.getRectangle()->getPixels(); i++) {
            float target_color[3];
            float old_target_color[3]{};

            target_color[0] = *channel0_input_iterator++;
            target_color[1] = *channel1_input_iterator++;
            target_color[2] = *channel2_input_iterator++;

            old_target_color[0] = target_color[0];
            old_target_color[1] = target_color[1];
            old_target_color[2] = target_color[2];

            target_color[0] += filter0_error;
            target_color[1] += filter1_error;
            target_color[2] += filter2_error;

            int amin_delta = 0;
            float best_delta = infinity;
            for (int d = 0; d < num_colors; d++) {

                float palette_color[3]{};
                palette_color[0] = palette_channel0[d];
                palette_color[1] = palette_channel1[d];
                palette_color[2] = palette_channel2[d];

                float delta = space->distance(space.get(), target_color, palette_color);
                if (delta < best_delta) {
                    amin_delta = d;
                    best_delta = delta;
                }
            }
            float final_color[3];
            final_color[0] = palette_channel0[amin_delta];
            final_color[1] = palette_channel1[amin_delta];
            final_color[2] = palette_channel2[amin_delta];

            filter0_error=filter0.filter(final_color[0] - old_target_color[0]);
            filter1_error=filter1.filter(final_color[1] - old_target_color[1]);
            filter2_error=filter2.filter(final_color[2] - old_target_color[2]);

            *(channel0_output_iterator++) = final_color[0];
            *(channel1_output_iterator++) = final_color[1];
            *(channel2_output_iterator++) = final_color[2];
        }
    } else if(settings.stalg == none) {
        for (unsigned i = 0; i < channel0_input_iterator.getRectangle()->getPixels(); i++) {
            *channel0_output_iterator++ = *channel0_input_iterator++;
            *channel1_output_iterator++ = *channel1_input_iterator++;
            *channel2_output_iterator++ = *channel2_input_iterator++;
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
    auto rectangle0 = channel0_output_iterator.getRectangle();
    auto rectangle1 = channel1_output_iterator.getRectangle();
    auto rectangle2 = channel2_output_iterator.getRectangle();
    rectangle0->depad();
    rectangle1->depad();
    rectangle2->depad();

    //Do colorspace conversion and setup float buffers and path walking
    for (int i = 0; i < num_pixels; i++) {
        float final_color[3];
        float output_color[3];
        final_color[0] = rectangle0->get(i);
        final_color[1] = rectangle1->get(i);
        final_color[2] = rectangle2->get(i);
        dither_to_output_colorspace.convert(dither_to_output_colorspace, final_color, output_color);
        float_triple_to_bytes(output_color, &output_image[output_image_offset]);
        output_image_offset += 3;
    }
}
