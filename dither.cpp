#include <limits>
#include <math.h>
#include <fstream>
#include <cmath>
#include <cstring>
#include <memory>
#include <sstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <chrono>

#define TAU 6.283185307179586

#define ERROR_LENGTH 7

enum algorithm {errordiffuse, ordered};

typedef std::shared_ptr<unsigned char[]> image_buffer_t;

struct ppm_file_info {
    char magic_num[3];
    int width;
    int height;
    int max_val;
};

struct dither_settings {
    algorithm stalg;
    float l_offset;
    float l_scale; // -inf to +inf
    float hue_phase;
    float hue_scale;
    float a_offset;
    float b_offset;
    float dither_intensity;
};

struct palette_info {
    int num_colors;
    std::shared_ptr<float[]> L;
    std::shared_ptr<float[]> a;
    std::shared_ptr<float[]> b;
};

void set_dither_defaults(dither_settings& settings) {
    //-ALGORITHM TYPE-//
    settings.stalg = errordiffuse;

    // --v-- color settings --v-- //
    settings.l_offset = 0.0;
    settings.l_scale = 1.0; // -inf to +inf

    settings.hue_phase = 0.0; // 0.00 to 1.00 (cyclic)
    settings.hue_scale = 1.0;

    settings.a_offset = 0.0;
    settings.b_offset = 0.0;
    // --v-- dither settings --v-- //

    settings.dither_intensity = 0.75;
    // --------------------------- //
}

// float -> unsigned char; clamps input between 0 and 255, then casts to unsigned char
unsigned char cclamp(float n) {
    if (n <= 0) return (unsigned char)0;
    if (n >= 255) return (unsigned char)255;
    return (unsigned char)std::roundf(n);
}

// float[3], float[3] -> float; euclidean distance between two 3d points
float hypot(float a[3], float b[3]) {
    const float x = b[0] - a[0];
    const float y = b[1] - a[1];
    const float z = b[2] - a[2];
    return x * x + y * y + z * z;
}

// float -> float; converts an srgb value to linear srgb
float linearize(float x) {
    return powf((x + 0.055) * 0.94786729857819905, 2.4);
}

// float -> float; converts a linear srgb value to srgb
float unlinearize(float x) {
    return powf(x, 0.41666666666666667) * 1.055 - 0.055;
}

// unsigned char[3] -> float[3]; converts an srgb input to oklab
void srgb_oklab(unsigned char rgb[3], float lab[3]) {
    // srgb -> linear srgb
    const float a = linearize(float(rgb[0]) * 0.00392156862745098);
    const float b = linearize(float(rgb[1]) * 0.00392156862745098);
    const float c = linearize(float(rgb[2]) * 0.00392156862745098);
    // linear srgb -> oklab
    const float d = powf(0.4122214708 * a + 0.5363325363 * b + 0.0514459929 * c, 0.3333333333333333);
    const float e = powf(0.2119034982 * a + 0.6806995451 * b + 0.1073969566 * c, 0.3333333333333333);
    const float f = powf(0.0883024619 * a + 0.2817188376 * b + 0.6299787005 * c, 0.3333333333333333);
    lab[0] = 0.2104542553 * d + 0.7936177850 * e - 0.0040720468 * f;
    lab[1] = 1.9779984951 * d - 2.4285922050 * e + 0.4505937099 * f;
    lab[2] = 0.0259040371 * d + 0.7827717662 * e - 0.8086757660 * f;
}

// float[3] -> unsigned char[3]; converts an oklab input to srgb
void oklab_srgb(float lab[3], unsigned char rgb[3]) {
    // oklab -> linear srgb
    float a = lab[0] + 0.3963377774 * lab[1] + 0.2158037573 * lab[2];
    float b = lab[0] - 0.1055613458 * lab[1] - 0.0638541728 * lab[2];
    float c = lab[0] - 0.0894841775 * lab[1] - 1.2914855480 * lab[2];
    a = a * a * a; //std::pow(a, 3);
    b = b * b * b; //std::pow(b, 3);
    c = c * c * c; //std::pow(c, 3);
    // linear srgb -> srgb
    rgb[0] = cclamp(unlinearize(4.0767416621 * a - 3.3077115913 * b + 0.2309699292 * c) * 255);
    rgb[1] = cclamp(unlinearize(-1.2684380046 * a + 2.6097574011 * b - 0.3413193965 * c) * 255);
    rgb[2] = cclamp(unlinearize(-0.0041960863 * a - 0.7034186147 * b + 1.7076147010 * c) * 255);
}

image_buffer_t create_image_buffer(int width, int height) {
    int num_pixels = width * height;
    return std::shared_ptr<unsigned char[]>(new unsigned char[num_pixels * 3]);
}

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

void load_palette(const std::string& palette_ppm_filename, palette_info& palette) {

    ppm_file_info file_info;
    image_buffer_t palette_buffer = load_ppm_image(palette_ppm_filename, file_info);

    int num_colors = file_info.width * file_info.height;

    palette.num_colors = num_colors;
    palette.L = std::shared_ptr<float[]>(new float[palette.num_colors]);
    palette.a = std::shared_ptr<float[]>(new float[palette.num_colors]);
    palette.b = std::shared_ptr<float[]>(new float[palette.num_colors]);

    memset(palette.L.get(), 0, sizeof(float) * num_colors);
    memset(palette.a.get(), 0, sizeof(float) * num_colors);
    memset(palette.b.get(), 0, sizeof(float) * num_colors);

    unsigned palette_buffer_offset = 0;
    for (int i = 0; i < num_colors; i++) {
        unsigned char rgb[3]{};
        float lab[3];

        rgb[0] = palette_buffer[palette_buffer_offset++];
        rgb[1] = palette_buffer[palette_buffer_offset++];
        rgb[2] = palette_buffer[palette_buffer_offset++];
        srgb_oklab(rgb, lab);

        palette.L[i] = lab[0];
        palette.a[i] = lab[1];
        palette.b[i] = lab[2];
    }
}

void dither(const dither_settings& settings,
    image_buffer_t input_image,
    image_buffer_t output_image,
    const palette_info& palette,
    int width,
    int height) {

    int num_pixels = width * height;
    float infinity = std::numeric_limits<float>::infinity();

    const float hue_phase_cos = std::cos(settings.hue_phase * TAU);
    const float hue_phase_sin = std::sin(settings.hue_phase * TAU);
    const float l_scale = settings.l_scale;
    const float hue_phase_cos_scale = hue_phase_cos * settings.hue_scale;
    const float hue_phase_sin_scale = hue_phase_sin * settings.hue_scale;
    const float l_offset = settings.l_offset;
    const float a_offset = settings.a_offset;
    const float b_offset = settings.b_offset;

    unsigned input_image_offset = 0;
    unsigned output_image_offset = 0;
    auto palette_L = palette.L.get();
    auto palette_a = palette.a.get();
    auto palette_b = palette.b.get();
    int num_colors = palette.num_colors;

    if (settings.stalg == errordiffuse) {
        const float negative_dither_intensity = -settings.dither_intensity;
        float weights[ERROR_LENGTH]{};
        for (int i = 0; i < ERROR_LENGTH; i++) {
            weights[i] = 1.0 / (i + 1) * negative_dither_intensity;
        }

        float errors_L[ERROR_LENGTH]{};
        float errors_a[ERROR_LENGTH]{};
        float errors_b[ERROR_LENGTH]{};
        for (int i = 0; i < ERROR_LENGTH; i++) {
            errors_L[i] = 0.0;
            errors_a[i] = 0.0;
            errors_b[i] = 0.0;
        }

        for (int i = 0; i < num_pixels; i++) {
            float lab[3];
            srgb_oklab(&input_image[input_image_offset], lab);
            input_image_offset += 3;

            float old_lab1 = lab[1];
            lab[0] = l_offset + (lab[0] - 0.5) * l_scale + 0.5;
            lab[1] = a_offset + (lab[1] * hue_phase_cos_scale - lab[2] * hue_phase_sin_scale);
            lab[2] = b_offset + (lab[2] * hue_phase_cos_scale + old_lab1 * hue_phase_sin_scale);

            float old_lab[3]{};
            old_lab[0] = lab[0];
            old_lab[1] = lab[1];
            old_lab[2] = lab[2];
            for (int m = 0; m < ERROR_LENGTH; m++) {
                lab[0] += errors_L[m] * weights[m];
                lab[1] += errors_a[m] * weights[m];
                lab[2] += errors_b[m] * weights[m];
            }

            int amin_delta = 0;
            float best_delta = infinity;
            for (int d = 0; d < num_colors; d++) {

                float p_lab[3]{};
                p_lab[0] = palette_L[d];
                p_lab[1] = palette_a[d];
                p_lab[2] = palette_b[d];

                float delta = hypot(lab, p_lab);
                if (delta < best_delta) {
                    amin_delta = d;
                    best_delta = delta;
                }
            }

            float new_lab[3]{};
            unsigned char new_rgb[3]{};
            new_lab[0] = palette_L[amin_delta];
            new_lab[1] = palette_a[amin_delta];
            new_lab[2] = palette_b[amin_delta];

            oklab_srgb(new_lab, &output_image[output_image_offset]);
            output_image_offset += 3;

            for (int m = ERROR_LENGTH - 1; m > 0; m--) {
                errors_L[m] = errors_L[m - 1];
                errors_a[m] = errors_a[m - 1];
                errors_b[m] = errors_b[m - 1];
            }
            errors_L[0] = new_lab[0] - old_lab[0];
            errors_a[0] = new_lab[1] - old_lab[1];
            errors_b[0] = new_lab[2] - old_lab[2];
        }
    }
    else if (settings.stalg == ordered) {
        const int matrix_size = 4;
        const int matrix[matrix_size][matrix_size] = { {0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5} };

        // TODO: Ordered algorithm fix

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float lab[3];
                srgb_oklab(&input_image[(y * width + x) * 3], lab);

                // Find the closest color in the palette to the pixel color
                float min_dist = std::numeric_limits<float>::infinity();
                int min_idx = -1;
                for (int i = 0; i < num_colors; i++) {
                    float L_diff = lab[0] - palette_L[i];
                    float a_diff = lab[1] - palette_a[i];
                    float b_diff = lab[2] - palette_b[i];
                    float dist = std::sqrt(L_diff * L_diff + a_diff * a_diff + b_diff * b_diff);
                    if (dist < min_dist) {
                        min_dist = dist;
                        min_idx = i;
                    }
                }

                // Dither using the closest color in the palette
                int ix = x % matrix_size;
                int iy = y % matrix_size;
                float threshold = (matrix[iy][ix] + 0.5) / (matrix_size * matrix_size);
                float l = lab[0];
                if (l <= threshold) {
                    l = palette_L[min_idx];
                }
                else {
                    l = palette_L[min_idx] + 1.0 / 65535.0; // Add a small amount to avoid quantization artifacts
                }
                lab[0] = l;
                lab[1] = palette_a[min_idx] + settings.dither_intensity * (lab[1] - palette_a[min_idx]);
                lab[2] = palette_b[min_idx] + settings.dither_intensity * (lab[2] - palette_b[min_idx]);

                oklab_srgb(lab, &output_image[(y * width + x) * 3]);
            }
        }
    }
}

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

    std::stringstream palette_cmd;
    palette_cmd << "ffmpeg -loglevel quiet -i \"palettes/" << color_palette << ".png\" palette.ppm -y";
    system(palette_cmd.str().c_str());

    palette_info palette;
    load_palette("palette.ppm", palette);

    dither_settings settings;
    set_dither_defaults(settings);
    settings.stalg = a;

    ppm_file_info file_info;
    std::stringstream d;
    d << "ffmpeg -loglevel quiet -i " << filen << " dither_input.ppm -y";
    system(d.str().c_str());
    image_buffer_t input_image = load_ppm_image("dither_input.ppm", file_info);
    image_buffer_t output_image = create_image_buffer(file_info.width, file_info.height);

    dither(settings, input_image, output_image, palette, file_info.width, file_info.height);

    save_ppm_image("dither_output.ppm", file_info, output_image);
    std::stringstream df;
    df << "ffmpeg -loglevel quiet -i dither_output.ppm " << out << ".png -y && del *.ppm";
    system(df.str().c_str());
    
    if (showimg) {
        std::stringstream sh;
        sh << out << ".png";
        system(sh.str().c_str());
    }
    return 0;
}
