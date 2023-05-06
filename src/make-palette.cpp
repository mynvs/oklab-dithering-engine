#include <fstream>
#include <cmath>
#include <cstring>
#include <sstream>

int main() {
    const int RED = 2;
    const int GRN = 14;
    const int BLU = 2;

    const int num_colors = RED*GRN*BLU;
    const double val_GB = 1.0/(GRN*BLU);
    const double val_B = 1.0/(BLU);

    std::ofstream output("palette.ppm", std::ios_base::binary);
    output << "P6\n" << GRN*BLU << " " << RED << "\n" << 255 << std::endl;

    unsigned char rgb[3];

    for(int i=0; i<num_colors; i++) {
        if (RED <= 1) {
            rgb[0] = 127;
        } else {
            const int n_R = int(std::floor(i*val_GB)) % RED;
            rgb[0] = std::round(255.0*n_R/(RED-1));
        }
        
        if (GRN <= 1) {
            rgb[1] = 127;
        } else {
            const int n_G = int(std::floor(i*val_B)) % GRN;
            rgb[1] = std::round(255.0*n_G/(GRN-1));
        }

        if (BLU <= 1) {
            rgb[2] = 127;
        } else {
            const int n_B = i % BLU;
            rgb[2] = std::round(255.0*n_B/(BLU-1));
        }

        output << rgb[0] << rgb[1] << rgb[2];
    }

    output.close();

    std::stringstream palette_cmd;
    palette_cmd << "ffmpeg -loglevel quiet -i palette.ppm palettes\\" << num_colors << ".png -y && del *.ppm";
    system(palette_cmd.str().c_str());
    return 0;
}