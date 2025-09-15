/* Interface for dealing with image files (via stb_image) */
#include <string>
#include "imghandle.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"

// lol
unsigned char* load_image(const std::string& input_name, image_info& file_info) {
    return stbi_load(input_name.c_str(), &file_info.width, &file_info.height, &file_info.channels, 3);
}

// lol 2
int save_image(const std::string& output_name, const image_info& file_info, unsigned char* image) {
    return stbi_write_png(output_name.c_str(), file_info.width, file_info.height, 3, image, file_info.width * 3);
}
