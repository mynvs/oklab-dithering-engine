/* Interface for dealing with images as image buffers */

#include "image_buffer.h"

image_buffer_t create_image_buffer(int width, int height) {
    int num_pixels = width * height;
    return std::shared_ptr<unsigned char[]>(new unsigned char[num_pixels * 3]);
}