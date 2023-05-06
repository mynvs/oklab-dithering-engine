/* Interface for dealing with PPM graphics files */
#include "image_buffer.h"

#ifndef PPM_H
#define PPM_H 1

struct ppm_file_info {
    char magic_num[3];
    int width;
    int height;
    int max_val;
};

image_buffer_t load_ppm_image(const std::string& input_name, ppm_file_info& file_info);

void save_ppm_image(const std::string& output_name, const ppm_file_info& file_info, image_buffer_t image);

#endif
