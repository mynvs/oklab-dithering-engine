#include <cstring>
#include <iostream>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#define DEL "del"
#else
#include <unistd.h>
#include <sys/stat.h>
#define DEL "rm"
#endif

#include "DitherEngineConfig.h"
#include "color_space.h"
#include "dither.h"
#include "ppm.h"



int main(int argc, char* argv[]) {
    const char* color_palette = "12";
    const char* filen = "input.png";
    const char* out = "dither";
    algorithm a = errordiffuse;
    dither_settings settings {};
    bool showimg = false;
    bool schng = false;
    printf("Dithering Engine version %d.%2.2d\n", DITHERING_ENGINE_VERSION_MAJOR, DITHERING_ENGINE_VERSION_MINOR);
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-i")
        {
            filen = argv[++i];
        }
        else if (std::string(argv[i]) == "-c")
        {
            struct stat buffer {};
            std::stringstream ext;
            ext <<"palettes/"<<argv[++i]<<".png";
            if (stat(ext.str().c_str(), &buffer) == 0) {
                color_palette = argv[i];
                printf("Using \u001b[90m\"\u001b[0m%s\u001b[90m\"\u001b[0m\u001b[3m color palette\u001b[0m.\n", argv[i]);
            }
            else {
                printf("Palette \u001b[90m\"\u001b[0m%s\u001b[90m\"\u001b[0m\u001b[31m doesn't exist\u001b[0m.\n", argv[i]);
                return 1;
            }
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
        else if (std::string(argv[i]) == "-p") {
            std::string dt = argv[++i];
            if (dt.compare("alien") == 0) {
                settings.l_offset = -0.3f;
                settings.l_scale = 2.5f;
                settings.hue_phase = 0.825f;
            }
            else if (dt.compare("invert") == 0) {
                settings.l_scale = -1.0f;
                settings.hue_scale = -1.0;
            }
            settings.dither_intensity = 0.75;
            settings.stalg = a;
            schng = true;
        }
        else if (std::string(argv[i]) == "--show") {
            showimg = true;
        }
    }

    struct stat buffer {};
    if (std::string(filen) == "input.png" && ~stat("./input.png", &buffer) == 0) {
        printf("There is\u001b[31m no input file\u001b[0m in working directory.\n");
        return 1;
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

    if(!schng) set_dither_defaults(settings);
    if (a != errordiffuse) {
        settings.stalg = a;
    }

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
