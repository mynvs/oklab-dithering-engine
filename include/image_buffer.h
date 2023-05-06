/* Interface for dealing with images as image buffers */

#include <memory>

#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H 1

typedef std::shared_ptr<unsigned char[]> image_buffer_t;

image_buffer_t create_image_buffer(int width, int height);

#endif
