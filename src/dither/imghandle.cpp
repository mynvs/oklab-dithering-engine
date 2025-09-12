/* Interface for dealing with image files (via stb_image) */
#include <string>
#include "imghandle.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"

image_buffer_t load_image(const std::string& input_name, image_info& file_info) {
    unsigned char* inp = stbi_load(input_name.c_str(), &file_info.width, &file_info.height, &file_info.channels, 3);
    int num_pixels = file_info.width * file_info.height;
    image_buffer_t return_value = create_image_buffer(file_info.width, file_info.height);

    for (int i = 0; i < num_pixels * 3; i++) {
        return_value[i] = inp[i];
    }

    delete inp;
    return return_value;
}

// lol
void save_image(const std::string& output_name, const image_info& file_info, image_buffer_t image) {
    stbi_write_png(output_name.c_str(), file_info.width, file_info.height, 3, image.get(), file_info.width * 3);
}
