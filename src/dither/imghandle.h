/* Interface for dealing with PPM graphics files */
#include <string>

#ifndef IMGHANDLE_H
#define IMGHANDLE_H 1

struct image_info {
    int width;
    int height;
    int channels;
};

unsigned char* load_image(const std::string& input_name, image_info& file_info);

int save_image(const std::string& output_name, const image_info& file_info, unsigned char* image);

#endif
