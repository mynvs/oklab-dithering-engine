#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "external/stb_image_resize2.h"

int main(int argc, char* argv[]) {
    const char* filen = "input.png";
    const char* out = "input.png";
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-i")
            filen = argv[++i];
        else if (std::string(argv[i]) == "-o")
            out = argv[++i];
    }

    int width, height, channels;
    unsigned char* inp = stbi_load(filen, &width, &height, &channels, 3);

    int rw = 3 * width / 3,
    rh = 3 * height / 3;

    unsigned char* subpdt = new unsigned char[width * height * 3];
    for (int y = 0; y < rh; ++y) {
        for (int x = 0; x < rw; ++x) {
            int idx = (y * width + x) * 3;
            int ridx = (y * rw + x) * 3;
            switch (x % 3) {
                case 0:
                    subpdt[ridx] = inp[idx];
                    subpdt[ridx+1] = 0;
                    subpdt[ridx+2] = 0;
                    break;
                case 1:
                    subpdt[ridx] = 0;
                    subpdt[ridx+1] = inp[idx+1];
                    subpdt[ridx+2] = 0;
                    break;
                case 2:
                    subpdt[ridx] = 0;
                    subpdt[ridx+1] = 0;
                    subpdt[ridx+2] = inp[idx+2];
                    break;
            }
        }
    }
    stbi_image_free(inp);

    int oh = rh / 3, ow = rw / 3;
    unsigned char* rsize = new unsigned char[width * oh * 3];
    stbir_resize(subpdt, width, height, 0,
                rsize, width, oh, 0, STBIR_RGB,
                STBIR_TYPE_UINT8, STBIR_EDGE_ZERO,
                STBIR_FILTER_CATMULLROM
    );

    unsigned char* outp = new unsigned char[ow * oh * 3];
    for (int y = 0; y < oh; ++y) {
        for (int x = 0; x < ow; ++x) {
            int si = (y * width + 3*x) * 3;
            int di = (y * ow + x) * 3;
            outp[di]   = rsize[si];
            outp[di+1] = rsize[si + 4];
            outp[di+2] = rsize[si + 8];
        }
    }

    stbi_write_png(out, ow, oh, 3, outp, ow * 3);
    printf("done\n");
    delete[] outp;
    delete[] rsize;
    delete[] subpdt;
    return 0;
}
