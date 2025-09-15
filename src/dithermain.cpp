#include <cmath>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <string>
#include <vector>

#ifndef __GNUC__
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "dither/color_space.h"
#include "path/path_choices.h"
#include "dither/dither.h"
#include "dither/imghandle.h"

#define SHOW_IMAGE (1)
#define SETTINGS_CHANGED (1 << 1)
#define DITHER_BURST (1 << 2)

int main(int argc, char* argv[]) {
    const char* color_palette = "win256";
    const char* filen = "input.png";
    const char* out = "dither";
    const char* pathname = "sequential";
    const char* color_space = "oklab";

    algorithm a = errordiffuse;
    dither_settings settings {};
    set_dither_defaults(settings);
    oklab_settings color_settings {};
    set_oklab_defaults(color_settings);
    unsigned char flags = 0;
    std::vector<std::string> palettes;

    printf("Dithering Engine \u001b[90mv\u001b[34;1m%.3d\u001b[0m\n", DITHERVERSION);
    for (int i = 1; i < argc; i++) {
        if (!std::strcmp(argv[i], "-i") && (i+1) < argc)
        {
            filen = argv[++i];
        }
        else if (!std::strcmp(argv[i], "-o") && (i+1) < argc)
        {
            out = argv[++i];
        }
        else if (!std::strcmp(argv[i], "-c") && (i+1) < argc)
        {
            struct stat buffer {};
            std::stringstream ext; ext <<"palettes/"<<argv[++i]<<".png";
            if (stat(ext.str().c_str(), &buffer) == 0) {
                color_palette = argv[i];
                printf("Using \u001b[90m\"\u001b[0m%s\u001b[90m\"\u001b[0m\u001b[3m color palette\u001b[0m.\n", argv[i]);
            }
            else {
                printf("Palette \u001b[90m\"\u001b[0m%s\u001b[90m\"\u001b[0m\u001b[31m doesn't exist\u001b[0m.\n", argv[i]);
                return 1;
            }
        }
        else if (!std::strcmp(argv[i], "-a") && (i+1) < argc)
        {
            std::string dt = argv[++i];

            if (dt.compare("none") == 0)
            {
                a = none;
            }
            else if (dt.compare("errordiffuse") == 0)
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
        else if (!std::strcmp(argv[i], "-f") && (i+1) < argc) {
            std::string dt = argv[++i];
            if (dt.compare("ramp") == 0) {
                settings.errorFilter = filter_ramp;
            }
            else if (dt.compare("decimate") == 0) {
                settings.errorFilter = filter_decimate;
                settings.dither_intensity = 1.0;

            }
            flags |= SETTINGS_CHANGED;
        }
        else if (!std::strcmp(argv[i], "-p") && (i+1) < argc) {
            std::string dt = argv[++i];
            if (dt.compare("alien") == 0) {
                color_settings.l_offset = -0.3f;
                color_settings.l_scale = 2.5f;
                color_settings.hue_cos_scale = std::cos(.825f);
                color_settings.hue_sin_scale = std::sin(.825f);
            }
            else if (dt.compare("invert") == 0) {
                color_settings.l_scale = -1.0f;
                color_settings.hue_cos_scale = -1.0;
                color_settings.hue_sin_scale = 0.0;
            }
            settings.dither_intensity = 0.75;
            settings.stalg = a;
            flags |= SETTINGS_CHANGED;
        }
        else if (!std::strcmp(argv[i], "--show")) {
            flags |= SHOW_IMAGE;
        }
        else if (!std::strcmp(argv[i], "--allp")) {
             flags |= DITHER_BURST;
            for (const auto& entry : std::filesystem::directory_iterator("palettes")) {
                palettes.push_back(entry.path().stem().string());
            }
        }
        else if (!std::strcmp(argv[i], "-d") && (i+1) < argc) {
            float decimation = 0.0;
            if(i+1 < argc) {
                try {
                    decimation = std::stof(argv[++i], NULL);
                } catch(const std::logic_error &e) {
                    //decimation should still be set to illegal 0.0 value
                    printf("EXCEPTION");
                }
            }
            if(decimation < 1.0) {
                printf("Must specify a valid floating point number for decimation >= 1.0\n");
                exit(1);
            }
            settings.decimation = decimation;
            flags |= SETTINGS_CHANGED;
        }
        else if (!std::strcmp(argv[i], "--show")) {
            if(i+1 < argc) {
                pathname = argv[++i];
                flags |= SETTINGS_CHANGED;
            } else {
                pathname = "";
            }
        }
        else if (!std::strcmp(argv[i], "--dfa")) {
            float decimation_filter_adjust=2.0;
            if(i+1 < argc) {
                try {
                    decimation_filter_adjust = std::stof(argv[++i], NULL);
                } catch(const std::logic_error &e) {
                    //decimation_filter_adjust Should still be set to illegal 2.0 value
                }
            }
            if(decimation_filter_adjust > 1.0) {
                printf("Must specify a valid floating point number after dfa <= 1.0\n");
                exit(1);
            }
            settings.decimation_filter_adjust = decimation_filter_adjust;
            flags |= SETTINGS_CHANGED;
        }
        else if (!std::strcmp(argv[i], "--color_space")) {
            if(i+1 < argc) {
                color_space = argv[++i];
            } else {
                color_space = "";
            }
        }
        else if (!std::strcmp(argv[i], "--luminance_offset")) {
            float luminance_offset = std::numeric_limits<float>::infinity();
            if(i+1 < argc) {
                try {
                    luminance_offset = std::stof(argv[++i], NULL);
                } catch(const std::logic_error &e) {

                }
            }

            if(!std::isinf(luminance_offset)) {
                color_settings.l_offset += luminance_offset;
            } else {
                printf("--luminance_offset reguires a floating point argument.\n");
                exit(1);
            }
        }
        else if (!std::strcmp(argv[i], "--luminance_scale")) {
            float luminance_scale = std::numeric_limits<float>::infinity();
            if(i+1 < argc) {
                try {
                    luminance_scale = std::stof(argv[++i], NULL);
                } catch(const std::logic_error &e) {

                }

            }
            if(!std::isinf(luminance_scale)) {
                color_settings.l_scale *= luminance_scale;
            } else {
                printf("--luminance_scale reguires a floating point argument.\n");
                exit(1);
            }
        }
        else if (!std::strcmp(argv[i], "--hcl_distance_weights")) {
            float hue_distance_weight = std::numeric_limits<float>::infinity();
            float chroma_distance_weight = std::numeric_limits<float>::infinity();
            float luminance_distance_weight = std::numeric_limits<float>::infinity();
            if(i+3 < argc) {
                try {
                    hue_distance_weight = std::stof(argv[++i], NULL);
                    chroma_distance_weight = std::stof(argv[++i], NULL);
                    luminance_distance_weight = std::stof(argv[++i], NULL);
                } catch(const std::logic_error &e) {

                }
            }
            if(std::isinf(hue_distance_weight) || std::isinf(hue_distance_weight) || std::isinf(hue_distance_weight)) {
                printf("--hcl_distance_weights reguires 3 floating point numbers after it.\n");
                exit(1);
            } else {
                color_settings.hue_distance_weight = hue_distance_weight;
                color_settings.chroma_distance_weight = chroma_distance_weight;
                color_settings.luminance_distance_weight = luminance_distance_weight;
            }
        }
    }

    std::unique_ptr<Path> path = get_path(pathname);
    if(path != NULL) {
        settings.dither_path = std::move(path);
    } else {
        int pathCounter = 0;
        printf("Invalid path chosen. Valid paths are:\n");
        const char* foundName = path_choices[pathCounter].name;
        while(foundName != NULL) {
            printf("\t%s\n", foundName);
            foundName = path_choices[++pathCounter].name;
        }
        printf("\n");
        exit(1);
    }

    printf("Using path %s\n", pathname);

    auto dither_color_space = get_color_space(OKLAB);
    if (std::strcmp(color_space, "oklab")==0) {
        dither_color_space = get_color_space(OKLAB);
        dither_color_space->settings.oklab = color_settings;
    } else if(std::strcmp(color_space, "rgb")==0) {
        dither_color_space = get_color_space(RGB);
    } else {
        printf("Unknown colorspace, use 'oklab' or 'rgb'\n");
        exit(1);
    }
    if (!(flags & DITHER_BURST)) {
        if (initdither(color_palette, filen, out, a, settings, dither_color_space, (flags & SETTINGS_CHANGED)) == 1) {
            printf("Dither \u001b[31mfailed\u001b[0m.\n");
            return 2;
        }
        if ((flags & SHOW_IMAGE)) {
            std::stringstream sh; sh << out << ".png";
            system(sh.str().c_str());
        }
    }
    else {
        int sz = palettes.size();
        printf("Palettes: \u001b[90m%d\u001b[0m\n\n", sz);
        for (int i = 0; i < sz; i++) {
            printf("Dithering with palette \u001b[90m\"\u001b[0m%s\u001b[90m\"\u001b[0m\n", palettes[i].c_str());
            std::filesystem::create_directory("burstdither");
            std::stringstream outd; outd << "burstdither/" << palettes[i].c_str();
            if (initdither(palettes[i].c_str(), filen, outd.str().c_str(), a, settings, dither_color_space, (flags & SETTINGS_CHANGED)) == 1) {
                printf("Dither \u001b[31mfailed\u001b[0m.\n");
                return 2;
            }
        }
        if ((flags & SHOW_IMAGE)) {
#ifdef _WIN32
            system("explorer burstdither");
#endif
        }
    }
    printf("Complete!\n");
    return 0;
}

int initdither(const char* color_palette,
               const char* filen,
               const char* out,
               algorithm a,
               dither_settings &settings,
               std::shared_ptr<color_space> dither_color_space,
               bool schng) {

    struct stat buffer {};
    if (~stat(filen, &buffer) == 0) {
        printf("File \"%s\"\u001b[31m does not exist\u001b[0m.\n", filen);
        return 1;
    }

    auto input_color_space = get_color_space(RGB);
    auto palette_color_space = get_color_space(RGB);
    auto output_color_space = get_color_space(RGB);

    auto palette_to_dither_color_space_converter = get_color_space_converter(palette_color_space, dither_color_space);
    auto input_to_dither_color_space_converter = get_color_space_converter(input_color_space, dither_color_space);
    auto dither_to_output_color_space_converter = get_color_space_converter(dither_color_space, output_color_space);

    std::stringstream palette_cmd; palette_cmd << "palettes/" << color_palette << ".png";
    if (~stat(filen, &buffer) == 0) { // in case
        printf("Palette image \"%s\"\u001b[31m does not exist\u001b[0m.\n", filen);
        return 1;
    }

    palette_info palette;
    load_palette(palette_cmd.str(), *palette_to_dither_color_space_converter, palette);

    if (!schng) set_dither_defaults(settings);
    if (a != errordiffuse) {
        settings.stalg = a;
    }

    image_info file_info;
    unsigned char* input_image = load_image(filen, file_info);
    if(input_image == NULL){
        printf("failed to load image\n");
        exit(13);
    }

    unsigned char* output_image = new unsigned char[(file_info.width * file_info.height) * 3];

    dither(settings,
        input_image,
        output_image,
        *input_to_dither_color_space_converter,
        *dither_to_output_color_space_converter,
        palette,
        file_info.width,
        file_info.height
    );

    std::stringstream df; df << out << ".png";
    if(!save_image(df.str(), file_info, output_image)){
        printf("failed to save dithered image\n");
        exit(13);
    }
    delete[] input_image;
    delete[] output_image;
    return 0;
}
