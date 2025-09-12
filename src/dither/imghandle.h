/* Interface for dealing with PPM graphics files */
#include "image_buffer.h"

#ifndef IMGHANDLE_H
#define IMGHANDLE_H 1

struct image_info {
    int width;
    int height;
    int channels;
};

image_buffer_t load_image(const std::string& input_name, image_info& file_info);

void save_image(const std::string& output_name, const image_info& file_info, image_buffer_t image);

#endif
