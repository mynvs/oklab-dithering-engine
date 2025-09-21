#include "stdio.h"
#include "dither/color_space.h"

typedef struct {
    const char * name;
    float colors[3];
} color_info;

color_info colors[] = {
    {"white", {1.0, 1.0, 1.0}},
    {"red", {1.0, 0.0, 0.0}},
    {"green", {0.0, 1.0, 0.0}},
    {"blue", {0.0, 0.0, 1.0}},
    {"cyan", {0.0, 1.0, 1.0}},
    {"magenta", {1.0, 0.0, 1.0}},
    {"yellow", {1.0, 1.0, 0.0}},
    {"black", {0, 0, 0}}
};


int main(int argc, char* argv[]) {
    for(unsigned i=0; i<sizeof(colors)/sizeof(color_info); ++i) {
        float outp[3];
        float cmp[3];
        srgb_oklab(colors[i].colors, outp);
        srgb_oklab(outp, cmp);
        printf("Color: %s { %f, %f, %f }\n", colors[i].name, outp[0], outp[1], outp[2]);
        printf("Original Color: { %f, %f, %f }\n", cmp[0], cmp[1], cmp[2]);
    }
}
