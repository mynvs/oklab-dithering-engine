#include <fstream>
#include <cmath>
#include <cstring>
#include <sstream>
#ifdef _WIN32
#define DEL "del"
#else
#define DEL "rm"
#endif

int main(int argc, char* argv[]) {
    const char* filen = "input.png";
    const char* out = "input.png";
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-i")
        {
            filen = argv[++i];
        }
        else if (std::string(argv[i]) == "-o")
        {
            out = argv[++i];
        }
    }

    std::stringstream rf; rf << "ffmpeg -loglevel quiet -i " << filen << " subpixel_input.ppm -y";
    system(rf.str().c_str());
    std::ifstream input1("subpixel_input.ppm", std::ios_base::binary);

    char magic_num1[3];
    int width1 = 0, height1 = 0, max_val1 = 0;
    input1 >> magic_num1;
    input1 >> width1 >> height1 >> max_val1;
    input1.get();

    int temp_x = 3*int(std::floor(width1*0.3333333333333333));
    int temp_y = 3*int(std::floor(height1*0.3333333333333333));
    int overhang_x = width1 - temp_x;
    int overhang_y = height1 - temp_y;
    width1 = temp_x;
    height1 = temp_y;
    int num_pixels1 = width1*height1;

    std::ofstream output1("subpixel.ppm", std::ios_base::binary);
    output1 << magic_num1 << "\n" << width1 << " " << height1 << "\n" << max_val1 << std::endl;

    for (int i=0; i<num_pixels1; i++) {

        if (i%width1 >= temp_x-1) {
            if (overhang_x == 1) {
                input1.get(); input1.get();input1.get();
            } else if (overhang_x == 2) {
                input1.get(); input1.get(); input1.get(); input1.get(); input1.get(); input1.get();
            }
        }

        unsigned char old_rgb[3]{};
        unsigned char new_rgb[3]{};

        old_rgb[0] = input1.get();
        old_rgb[1] = input1.get();
        old_rgb[2] = input1.get();

        if ((i%width1)%3 == 0) {

            new_rgb[0] = old_rgb[0];
            new_rgb[1] = 0;
            new_rgb[2] = 0;
        } else if ((i%width1)%3 == 1) {
            new_rgb[0] = 0;
            new_rgb[1] = old_rgb[1];
            new_rgb[2] = 0;
        } else {
            new_rgb[0] = 0;
            new_rgb[1] = 0;
            new_rgb[2] = old_rgb[2];
        }
        output1 << new_rgb[0] << new_rgb[1] << new_rgb[2];
    }

    output1.close();
    input1.close();

    int height_2 = int(std::floor(height1*0.3333333333333333));
    std::stringstream ss;
    ss << "ffmpeg -loglevel quiet -i subpixel.ppm -vf scale=" << width1 << ":" << height_2 << ":flags='lanczos' subpixel_scaled.ppm -y";
    system(ss.str().c_str());

    std::ifstream input("subpixel_scaled.ppm", std::ios_base::binary);

    char magic_num[3];
    int width = 0, height = 0, max_val = 0;
    input >> magic_num;
    input >> width >> height >> max_val;
    input.get();

    int num_pixels = width*height;

    std::ofstream output("subpixel_output.ppm", std::ios_base::binary);
    output << magic_num << "\n" << int(std::floor(width*0.3333333333333333)) << " " << height << "\n" << max_val << std::endl;

    for (int i=0; i<num_pixels; i++) {
        unsigned char rgb[3]{};
        double lab[3]{};

        rgb[0] = input.get(); // R
        input.get();
        input.get();

        input.get();
        rgb[1] = input.get(); // G
        input.get();

        input.get();
        input.get();
        rgb[2] = input.get(); // B

        output << rgb[0] << rgb[1] << rgb[2];
    }

    output.close();
    input.close();

    std::stringstream dv; dv << "ffmpeg -loglevel quiet -i subpixel_output.ppm " << out << " -y &&"<< DEL <<" *.ppm";
    system(dv.str().c_str());

    return 0;
}