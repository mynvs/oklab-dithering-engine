/* Interface for dealing with PPM graphics files */
#include <iostream>
#include <fstream>
#include "ppm.h"

image_buffer_t load_ppm_image(const std::string& input_name, ppm_file_info& file_info) {

    std::ifstream input(input_name, std::ios_base::binary);
    input >> file_info.magic_num;
    input >> file_info.width >> file_info.height >> file_info.max_val;
    input.get();

    int num_pixels = file_info.width * file_info.height;
    image_buffer_t return_value = create_image_buffer(file_info.width, file_info.height);

    for (int i = 0; i < num_pixels * 3; i++) {
        return_value[i] = input.get();
    }

    input.close();
    return return_value;
}

void save_ppm_image(const std::string& output_name, const ppm_file_info& file_info, image_buffer_t image) {
    std::ofstream output(output_name, std::ios_base::binary);
    output << file_info.magic_num << "\n" << file_info.width << " " << file_info.height << "\n" << file_info.max_val << std::endl;

    int num_pixels = file_info.width * file_info.height;
    std::shared_ptr<unsigned char[]> return_value(new unsigned char[num_pixels * 3]);

    for (int i = 0; i < num_pixels * 3; i++) {
        output << image[i];
    }
    output.close();
}


