#include <math.h>
#include "color_space.h"
#include <iostream>

// float -> unsigned char; clamps input between 0 and 255, then casts to unsigned char
static uint8_t cclamp(float n) {
    if (n <= 0) return (unsigned char)0;
    if (n >= 255) return (unsigned char)255;
    return (uint8_t)std::roundf(n);
}

void byte_triple_to_floats(const uint8_t bytes[3], float floats[3]) {
    for( int i=0; i<3; i++) {
        floats[i] =  float(bytes[i]) * 0.00392156862745098;
    }
}
void float_triple_to_bytes(const float floats[3], uint8_t bytes[3]) {
    for( int i=0; i<3; i++) {
        bytes[i] =  cclamp( floats[i]*255 );
    }
}

// float -> float; converts an srgb value to linear srgb
static float linearize(float x) {
    if (x > 0.04045) {
        return powf((x + 0.055)*0.94786729857819905, 2.4);
    } else {
        return x * 0.07739938080495357;
    }
}

// float -> float; converts a linear srgb value to srgb
static float unlinearize(float x) {
    if( x > 0.0031308 ) {
        return powf(x, 0.41666666666666667)*1.055 - 0.055;
    } else {
        return 12.92 * x;
    }
}

float euclidean_distance(const color_space *color_space, float colorA[3], float colorB[3]) {
    float x = colorA[0]-colorB[0];
    float y = colorA[1]-colorB[1];
    float z = colorA[2]-colorB[2];
    return x*x + y*y + z*z;
}

float hcl_oklab_distance(const color_space *color_space, float colorA[3], float colorB[3]) {
    float hue_distance_weight = color_space->settings.oklab.hue_distance_weight;
    float chroma_distance_weight = color_space->settings.oklab.chroma_distance_weight;
    float luminance_distance_weight = color_space->settings.oklab.luminance_distance_weight;
    float l1 = colorA[0];
    float l2 = colorB[0];
    float a1 = colorA[1];
    float a2 = colorB[1];
    float b1 = colorA[2];
    float b2 = colorB[2];

    float deltaLuminance = l1 - l2;
    float chromaA = std::sqrt(a1*a1+b1*b1);
    float chromaB = std::sqrt(a2*a2+b2*b2);
    float deltaChroma = chromaA - chromaB;
    float deltaChromaSquared = deltaChroma * deltaChroma;
    float deltaA = a1-a2;
    float deltaB = b1-b2;
    float deltaHueSquared = deltaA*deltaA + deltaB*deltaB - deltaChromaSquared;
    return deltaLuminance*deltaLuminance * luminance_distance_weight + deltaChromaSquared * chroma_distance_weight + deltaHueSquared * hue_distance_weight;
}

// converts an srgb input to oklab
static void srgb_oklab(const color_space_converter &converter, const float rgb[3], float lab[3]) {
    // srgb -> linear srgb
    const float a = linearize(rgb[0]);
    const float b = linearize(rgb[1]);
    const float c = linearize(rgb[2]);
    // linear srgb -> oklab
    const float d = powf(0.4122214708 * a + 0.5363325363 * b + 0.0514459929 * c, 0.3333333333333333);
    const float e = powf(0.2119034982 * a + 0.6806995451 * b + 0.1073969566 * c, 0.3333333333333333);
    const float f = powf(0.0883024619 * a + 0.2817188376 * b + 0.6299787005 * c, 0.3333333333333333);
    lab[0] = 0.2104542553 * d + 0.7936177850 * e - 0.0040720468 * f;
    lab[1] = 1.9779984951 * d - 2.4285922050 * e + 0.4505937099 * f;
    lab[2] = 0.0259040371 * d + 0.7827717662 * e - 0.8086757660 * f;

    if(converter.post_process != NULL) {
        converter.post_process(*converter.destination, lab);
    }
}

// float[3] -> unsigned char[3]; converts an oklab input to srgb
static void oklab_srgb(const color_space_converter &converter, const float lab[3], float rgb[3]) {
    // oklab -> linear srgb
    float a = lab[0] + 0.3963377774 * lab[1] + 0.2158037573 * lab[2];
    float b = lab[0] - 0.1055613458 * lab[1] - 0.0638541728 * lab[2];
    float c = lab[0] - 0.0894841775 * lab[1] - 1.2914855480 * lab[2];
    a = a * a * a; //std::pow(a, 3);
    b = b * b * b; //std::pow(b, 3);
    c = c * c * c; //std::pow(c, 3);
    // linear srgb -> srgb
    rgb[0] = unlinearize(4.0767416621 * a - 3.3077115913 * b + 0.2309699292 * c);
    rgb[1] = unlinearize(-1.2684380046 * a + 2.6097574011 * b - 0.3413193965 * c);
    rgb[2] = unlinearize(-0.0041960863 * a - 0.7034186147 * b + 1.7076147010 * c);

    if(converter.post_process != NULL) {
        converter.post_process(*converter.destination, rgb);
    }
}

static void oklab_hue_shift(const color_space &space, float color[3]) {
    const oklab_settings &settings = space.settings.oklab;
    const auto l_offset = settings.l_offset;
    const auto a_offset = settings.a_offset;
    const auto b_offset = settings.b_offset;
    const auto l_scale = settings.l_scale;

    //hue_cos_scale  is the vector i value, hue_sin_scale is the vector j value
    const auto hue_cos_scale = settings.hue_cos_scale;
    const auto hue_sin_scale = settings.hue_sin_scale;

    float old_color_1 = color[1];
    color[0] = l_offset + color[0] * l_scale; /*(color[0] - 0.5) * l_scale + 0.5;*/
    color[1] = a_offset + (color[1] * hue_cos_scale - color[2] * hue_sin_scale);
    color[2] = b_offset + (color[2] * hue_cos_scale + old_color_1 * hue_sin_scale);
}


static void identity_convert(const color_space_converter &converter, const float source[3], float destination[3]) {
    for(int i=0; i<3; i++) {
        destination[i] = source[i];
    }
    if(converter.post_process != NULL) {
        converter.post_process(*converter.destination, destination);
    }
}

void set_rgb_defaults(rgb_settings& settings) {
    //Does nothing currently
}

void set_oklab_defaults(oklab_settings &settings) {
    // --v-- color settings --v-- //
    settings.l_offset = 0.0;
    settings.l_scale = 1.0; // -inf to +inf
    settings.a_offset = 0.0;
    settings.b_offset = 0.0;

    //Set i (cos) and j (sin) values for angle and magnitude vector
    settings.hue_cos_scale = 1.0;
    settings.hue_sin_scale = 0.0;

    //for calculation of deltaE colors, set weights for what is most important
    //hue, chroma, and luminance
    settings.hue_distance_weight = 1.0;
    settings.chroma_distance_weight = 1.0;
    settings.luminance_distance_weight = 1.0;
}

std::shared_ptr<color_space> get_color_space(color_space_type type) {
    auto return_value = std::make_shared<color_space>();
    return_value->type = type;

    if (type == RGB) {
        return_value->distance = euclidean_distance;
        set_rgb_defaults(return_value->settings.rgb);
    } else {
        set_oklab_defaults(return_value->settings.oklab);
        return_value->distance = hcl_oklab_distance;
    }
    return return_value;
}

std::shared_ptr<color_space_converter> get_color_space_converter(std::shared_ptr<color_space> source,
                                                                 std::shared_ptr<color_space> destination) {
    auto return_value = std::make_shared<color_space_converter>();
    return_value->source = source;
    return_value->destination = destination;
    return_value->post_process = NULL;

    if(source->type == RGB && destination->type == OKLAB) {
        return_value->convert = srgb_oklab;
    } else if (source->type == OKLAB && destination->type == RGB) {
        return_value->convert = oklab_srgb;
    } else if (source->type == destination->type) {
        return_value->convert = identity_convert;
    }
    if(destination->type == OKLAB) {
        return_value->post_process = oklab_hue_shift;
    }
    return return_value;
}

