/*
#include <limits>
#include <math.h>
#include <fstream>
#include <cmath>
#include <cstring>
#include <memory>
#include <sstream>
#include <iostream>
#include <chrono>
*/
#include <cstring>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define DEL "del"
#else
#include <unistd.h>
#define DEL "rm"
#endif

#include "color_space.h"
#include "dither.h"
#include "ppm.h"


int main(int argc, char* argv[]) {
    std::string color_palette = "12";
    const char* filen = "input.png";
    const char* out = "dither";
    algorithm a = errordiffuse;
    bool showimg = false;
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-i")
        {
            filen = argv[++i];
        }
        else if (std::string(argv[i]) == "-c")
        {
            color_palette = argv[++i];
        }
        else if (std::string(argv[i]) == "-o")
        {
            out = argv[++i];
        }
        else if (std::string(argv[i]) == "-a")
        {
            std::string dt = argv[++i];
            if (dt.compare("errordiffuse") == 0)
            {
                a = errordiffuse;
            }
            else if (dt.compare("ordered") == 0)
            {
                printf("Using 'ordered'\n");
                a = ordered;
            }
            else {
                printf("INVALID DITHERING ALGORITHM");
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--show") {
            showimg = true;
        }
    }

    auto input_color_space = get_color_space(RGB);
    auto palette_color_space = get_color_space(RGB);
    auto dither_color_space = get_color_space(OKLAB); 
    auto output_color_space = get_color_space(RGB); 

    auto palette_to_dither_color_space_converter = get_color_space_converter(palette_color_space, dither_color_space);
    auto input_to_dither_color_space_converter = get_color_space_converter(input_color_space, dither_color_space);
    auto dither_to_output_color_space_converter = get_color_space_converter(dither_color_space, output_color_space);

    std::stringstream palette_cmd;
    palette_cmd << "ffmpeg -loglevel quiet -i \"palettes/" << color_palette << ".png\" palette.ppm -y";
    system(palette_cmd.str().c_str());

    palette_info palette;
    load_palette("palette.ppm", *palette_to_dither_color_space_converter, palette);

    dither_settings settings;
    set_dither_defaults(settings);
    settings.stalg = a;

    ppm_file_info file_info;
    std::stringstream d;
    d << "ffmpeg -loglevel quiet -i " << filen << " dither_input.ppm -y";
    system(d.str().c_str());
    image_buffer_t input_image = load_ppm_image("dither_input.ppm", file_info);
    image_buffer_t output_image = create_image_buffer(file_info.width, file_info.height);

    dither(settings,
           input_image,
           output_image,
           *input_to_dither_color_space_converter,
           *dither_to_output_color_space_converter,
           palette,
           file_info.width,
           file_info.height);

    save_ppm_image("dither_output.ppm", file_info, output_image);
    std::stringstream df;
    df << "ffmpeg -loglevel quiet -i dither_output.ppm " << out << ".png -y && " << DEL << " *.ppm";
    system(df.str().c_str());
    
    if (showimg) {
        std::stringstream sh;
        sh << out << ".png";
        system(sh.str().c_str());
    }
    return 0;
}
