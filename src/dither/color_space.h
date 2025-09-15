#include <cstdint>
#include <memory>

#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H 1

struct oklab_settings {
    float l_offset;
    float l_scale; // -inf to +inf

    /* i and j vector values to multiply by
     * the hue to set its magnitude and rotation
     */
    float hue_cos_scale;
    float hue_sin_scale;
    float a_offset;
    float b_offset;

    //These are chosen to set how important hue / chroma / luminance
    //relative to each other
    float hue_distance_weight;
    float chroma_distance_weight;
    float luminance_distance_weight;

};


//EMPTY settings for RGB
struct rgb_settings {
};

enum color_space_type {
    RGB = 0,
    OKLAB = 1
};

void set_rgb_defaults(rgb_settings& settings);

void set_oklab_defaults(oklab_settings &settings);

struct color_space {
    color_space_type type;
    float (*distance)(const color_space *color_space, float colorA[3], float colorB[3]);
    union {
        oklab_settings oklab;
        rgb_settings rgb;
    } settings;
};

struct color_space_converter {
    void (*convert)(const color_space_converter &converter, const float source_color[3], float destination_color[3]);
    void (*post_process)(const color_space &color_space, float color[3]);
    std::shared_ptr<color_space> source;
    std::shared_ptr<color_space> destination;
};

void byte_triple_to_floats(const uint8_t bytes[3], float floats[3]);

void float_triple_to_bytes(const float floats[3], uint8_t bytes[3]);

std::shared_ptr<color_space> get_color_space(color_space_type);

std::shared_ptr<color_space_converter> get_color_space_converter(std::shared_ptr<color_space> source,
                                                                 std::shared_ptr<color_space> destination);

#endif
