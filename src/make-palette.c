#include "math.h"
#include <direct.h>
#include "utils/fileutils.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

int main(int argc, char* argv[]) {
    unsigned char RED = 2, GRN = 14, BLU = 2; // ALPHA disabled by default

    if(argc > 1) RED = atoi(argv[1]);
    if(argc > 2) GRN = atoi(argv[2]);
    if(argc > 3) BLU = atoi(argv[3]);

    const int num_colors = RED*GRN*BLU;
    const double val_GB = 1.0/(GRN*BLU);
    const double val_B = 1.0/(BLU);

    unsigned char* colorp = malloc(num_colors * 3);
    for(int i = 0; i < num_colors; i++) {
        colorp[i * 3] = (RED > 1)
            ? round(255.0 * ((int)floor(i*val_GB) % RED) / (RED-1))
            : 127;
        colorp[i * 3 + 1] = (GRN > 1)
            ? round(255.0 * ((int)floor(i*val_B) % GRN) / (GRN-1))
            : 127;
        colorp[i * 3 + 2] = (BLU > 1)
            ? round(255.0 * (i % BLU) / (BLU-1))
            : 127;
    }

    mkdir("palettes", 0775);

    stbi_write_png(concat_path_ext("palettes", int_to_charptr(num_colors), "png"), GRN*BLU, RED, 3, colorp, (GRN*BLU) * 3);
    printf("Colors: %d\n", num_colors);
    free(colorp);
    return 0;
}
