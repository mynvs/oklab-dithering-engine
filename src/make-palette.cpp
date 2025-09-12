#include <cmath>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

int main(int argc, char* argv[]) {
    int RED = 2, GRN = 14, BLU = 2;

    for (int i = 1; i < argc; i++) {
        switch(i){
            case 1:
                RED = std::stoi(argv[i]);
                break;
            case 2:
                GRN = std::stoi(argv[i]);
                break;
            case 3:
                BLU = std::stoi(argv[i]);
                break;
        }
    }

    const int num_colors = RED*GRN*BLU;
    const double val_GB = 1.0/(GRN*BLU);
    const double val_B = 1.0/(BLU);

    unsigned char rgb[3]{};
    unsigned char* colorp = new unsigned char[num_colors * 3];

    for(int i=0; i<num_colors; i++) {
        if (RED <= 1)
            rgb[0] = 127;
        else {
            const int n_R = int(std::floor(i*val_GB)) % RED;
            rgb[0] = std::round(255.0*n_R/(RED-1));
        }

        if (GRN <= 1)
            rgb[1] = 127;
        else {
            const int n_G = int(std::floor(i*val_B)) % GRN;
            rgb[1] = std::round(255.0*n_G/(GRN-1));
        }

        if (BLU <= 1)
            rgb[2] = 127;
        else {
            const int n_B = i % BLU;
            rgb[2] = std::round(255.0*n_B/(BLU-1));
        }

        colorp[i * 3] = rgb[0];
        colorp[i * 3 + 1] = rgb[1];
        colorp[i * 3 + 2] = rgb[2];
    }

    std::stringstream palette_cmd;
    palette_cmd << "palettes/" << num_colors << ".png";
    stbi_write_png(palette_cmd.str().c_str(), GRN*BLU, RED, 3, colorp, (GRN*BLU) * 3);
    printf("Colors: %d\n", num_colors);
    delete[] colorp;
    return 0;
}
