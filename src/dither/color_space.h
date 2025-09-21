#include "stdio.h"
#include "math.h"

#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H 1

static const float INT8_REC = (1.0f / 0xFF); // precompute division (float division is insanely slow)

typedef struct {
    float l_offset;
    float l_scale; // luminosity intensity

    // hue wheel wackiness
    float hue_cos_scale;
    float hue_sin_scale;
    float a_offset;
    float b_offset;

    //These are chosen to set how prominent they're relative to each other
    float hue_distance_weight;
    float chroma_distance_weight;
    float luminance_distance_weight;

} oklab_settings;

// float -> unsigned char; clamps input between 0 and 255, then casts to unsigned char
static unsigned char cclamp(float n) {
    if (n < 0) return 0;
    if (n > 255) return 255;
    return (unsigned char)roundf(n);
}

static void byte_triple_to_floats(unsigned char bytes[3], float floats[3]) {
    floats[0] = (float)bytes[0] * INT8_REC;
    floats[1] = (float)bytes[1] * INT8_REC;
    floats[2] = (float)bytes[2] * INT8_REC;
}

static void float_triple_to_bytes(float floats[3], unsigned char bytes[3]) {
    bytes[0] = cclamp(floats[0]*255);
    bytes[1] = cclamp(floats[1]*255);
    bytes[2] = cclamp(floats[2]*255);
}

// float -> float; converts an srgb value to linear srgb
static float linearize(float x) {
    return x > 0.04045f
        ? powf((x + 0.055)*0.94786729857819905, 2.4)
        : x * 0.07739938080495357;
}

// float -> float; converts a linear srgb value to srgb
static float unlinearize(float x) {
    return x > 0.0031308f
        ? powf(x, 0.41666666666666667)*1.055 - 0.055
        : 12.92 * x;
}

static float euclidean_distance(float a[3], float b[3]) {
    float x = a[0]-b[0], y = a[1]-b[1], z = a[2]-b[2];
    return x*x + y*y + z*z;
}

static float hcl_oklab_distance(const oklab_settings* settings, float a[3], float b[3]) {
    float deltaLuminance = a[0] - b[0];
    float chromaA = sqrt(a[1] * a[1] + a[2] * a[2]);
    float chromaB = sqrt(b[1] * b[1] + b[2] * b[2]);
    float deltaChroma = chromaA - chromaB;
    float deltaChromaSquared = deltaChroma * deltaChroma;
    float deltaA = a[1]-b[1];
    float deltaB = a[2]-b[2];
    float deltaHueSquared = deltaA*deltaA + deltaB*deltaB - deltaChromaSquared;
    return deltaLuminance*deltaLuminance * settings->luminance_distance_weight
        + deltaChromaSquared * settings->chroma_distance_weight
        + deltaHueSquared * settings->hue_distance_weight;
}

// converts an srgb input to oklab
static void srgb_oklab(const float rgb[3], float lab[3]) {
    // srgb -> linear srgb
    const float a = linearize(rgb[0]);
    const float b = linearize(rgb[1]);
    const float c = linearize(rgb[2]);
    // linear srgb -> oklab
    const float d = cbrtf(0.4122214708f * a + 0.5363325363f * b + 0.0514459929f * c);
    const float e = cbrtf(0.2119034982f * a + 0.6806995451f * b + 0.1073969566f * c);
    const float f = cbrtf(0.0883024619f * a + 0.2817188376f * b + 0.6299787005f * c);
    lab[0] = 0.2104542553f * d + 0.7936177850f * e - 0.0040720468f * f;
    lab[1] = 1.9779984951f * d - 2.4285922050f * e + 0.4505937099f * f;
    lab[2] = 0.0259040371f * d + 0.7827717662f * e - 0.8086757660f * f;
}

// float[3] -> unsigned char[3]; converts an oklab input to srgb
static void oklab_srgb(const float lab[3], float rgb[3]) {
    // oklab -> linear srgb
    float a = lab[0] + 0.3963377774 * lab[1] + 0.2158037573 * lab[2];
    float b = lab[0] - 0.1055613458 * lab[1] - 0.0638541728 * lab[2];
    float c = lab[0] - 0.0894841775 * lab[1] - 1.2914855480 * lab[2];
    a = a * a * a; // pow(a, 3);
    b = b * b * b; // pow(b, 3);
    c = c * c * c; // pow(c, 3);
    // linear srgb -> srgb
    rgb[0] = unlinearize(4.0767416621 * a - 3.3077115913 * b + 0.2309699292 * c);
    rgb[1] = unlinearize(-1.2684380046 * a + 2.6097574011 * b - 0.3413193965 * c);
    rgb[2] = unlinearize(-0.0041960863 * a - 0.7034186147 * b + 1.7076147010 * c);
}

static void oklab_hue_shift(const oklab_settings* settings, float color[3]) {
    float old_color_1 = color[1];
    color[0] = settings->l_offset + color[0] * settings->l_scale;
    color[1] = settings->a_offset +
        (color[1] * settings->hue_cos_scale - color[2] * settings->hue_sin_scale);
    color[2] = settings->b_offset +
        (color[2] * settings->hue_cos_scale + old_color_1 * settings->hue_sin_scale);
}

static void set_oklab_defaults(oklab_settings* settings) {
    // --v-- color settings --v-- //
    settings->l_offset = 0.0;
    settings->l_scale = 1.0; // -inf to +inf
    settings->a_offset = 0.0;
    settings->b_offset = 0.0;

    //Set i (cos) and j (sin) values for angle and magnitude vector
    settings->hue_cos_scale = 1.0;
    settings->hue_sin_scale = 0.0;

    //for calculation of deltaE colors, set weights for what is most important
    //hue, chroma, and luminance
    settings->hue_distance_weight = 1.0;
    settings->chroma_distance_weight = 1.0;
    settings->luminance_distance_weight = 1.0;
}

#endif
