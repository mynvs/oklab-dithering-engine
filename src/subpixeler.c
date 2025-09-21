#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "external/stb_image_resize2.h"

int main(int argc, char* argv[]) {
    const char* filen = "input.png";
    const char* out = "input.png";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && (i+1) < argc)
            filen = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && (i+1) < argc)
            out = argv[++i];
    }

    int width, height, channels;
    unsigned char* inp = stbi_load(filen, &width, &height, &channels, 4);

    int rw = 3 * width / 3,
    rh = 3 * height / 3;

    int oh = rh / 3, ow = rw / 3;
    unsigned char* rsize = (unsigned char*)malloc(width * oh * 4);
    stbir_resize(inp, width, height, 0,
                rsize, width, oh, 0, STBIR_RGBA_PM,
                STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP,
                STBIR_FILTER_CATMULLROM
    );

    stbi_image_free(inp);

    unsigned char* outp = (unsigned char*)malloc(ow * oh * 4);
    for (int y = 0; y < oh; ++y) {
        for (int x = 0; x < ow; ++x) {
            int si = (y * width + 3 * x) * 4;
            int di = (y * ow + x) * 4;
            outp[di] = rsize[si];
            outp[di+1] = rsize[si + 5];
            outp[di+2] = rsize[si + 10];
            outp[di+3] = (rsize[si + 3] + rsize[si + 7] + rsize[si + 11]) / 3; // average out!!!!!!!!!!!!!!!!!!!!
        }
    }

    stbi_write_png(out, ow, oh, 4, outp, ow * 4);
    printf("done\n");
    free(outp);
    free(rsize);
    return 0;
}
