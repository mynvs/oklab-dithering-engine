#include <fstream>
#include <cmath>
#include <cstring>
#include <sstream>

#define TAU 6.283185307179586

// double[3], double[3] -> double; euclidean distance between two 3d points
double hypot(double a[3], double b[3]) {
    const double x = b[0] - a[0];
    const double y = b[1] - a[1];
    const double z = b[2] - a[2];
    return x*x + y*y + z*z;
}

// double -> unsigned char; clamps input between 0 and 255, then casts to unsigned char
unsigned char cclamp(double n) {
    if (n <= 0) return (unsigned char) 0;
    if (n >= 255) return (unsigned char) 255;
    return (unsigned char) std::round(n);
}

// double -> double; converts an srgb value to linear srgb
double linearize(double x) {
    return std::pow((x + 0.055)*0.94786729857819905, 2.4);
}

// double -> double; converts a linear srgb value to srgb
double unlinearize(double x) {
    return std::pow(x, 0.41666666666666667)*1.055 - 0.055;
}

// unsigned char[3] -> double[3]; converts an srgb input to oklab
void srgb_oklab(unsigned char rgb[3], double lab[3]) {
    // srgb -> linear srgb
    const double a = linearize(double(rgb[0])*0.00392156862745098);
    const double b = linearize(double(rgb[1])*0.00392156862745098);
    const double c = linearize(double(rgb[2])*0.00392156862745098);
    // linear srgb -> oklab
    const double d = std::pow(0.4122214708*a + 0.5363325363*b + 0.0514459929*c, 0.3333333333333333);
	const double e = std::pow(0.2119034982*a + 0.6806995451*b + 0.1073969566*c, 0.3333333333333333);
	const double f = std::pow(0.0883024619*a + 0.2817188376*b + 0.6299787005*c, 0.3333333333333333);
    lab[0] = 0.2104542553*d + 0.7936177850*e - 0.0040720468*f;
    lab[1] = 1.9779984951*d - 2.4285922050*e + 0.4505937099*f;
    lab[2] = 0.0259040371*d + 0.7827717662*e - 0.8086757660*f;
}

// double[3] -> unsigned char[3]; converts an oklab input to srgb
void oklab_srgb(double lab[3], unsigned char rgb[3]) {
    // oklab -> linear srgb
    double a = lab[0] + 0.3963377774*lab[1] + 0.2158037573*lab[2];
    double b = lab[0] - 0.1055613458*lab[1] - 0.0638541728*lab[2];
    double c = lab[0] - 0.0894841775*lab[1] - 1.2914855480*lab[2];
    a = std::pow(a, 3);
    b = std::pow(b, 3);
    c = std::pow(c, 3);
    // linear srgb -> srgb
    rgb[0] = cclamp(unlinearize( 4.0767416621*a - 3.3077115913*b + 0.2309699292*c)*255);
    rgb[1] = cclamp(unlinearize(-1.2684380046*a + 2.6097574011*b - 0.3413193965*c)*255);
    rgb[2] = cclamp(unlinearize(-0.0041960863*a - 0.7034186147*b + 1.7076147010*c)*255);
}

int main() {


    // --v-- color settings --v-- //
    std::string color_palette = "8";

    double l_offset = 0.0;
    double l_scale = 1.0; // -inf to +inf

    double hue_phase = 0.0; // 0.00 to 1.00 (cyclic)
    double hue_scale = 1.0;

    double a_offset = 0.0;
    double b_offset = 0.0;
    // --v-- dither settings --v-- //

    const double dither_intensity = 0.64;
    const int error_length = 29;
    // --------------------------- //


    std::stringstream palette_cmd;
    palette_cmd << "ffmpeg -loglevel quiet -i \"palettes\\" << color_palette << ".png\" palette.ppm -y";
    system(palette_cmd.str().c_str());
    std::ifstream palette("palette.ppm", std::ios_base::binary);
    char magic_num2[3];
    int width2 = 0, height2 = 0, max_val2 = 0;
    palette >> magic_num2;
    palette >> width2 >> height2 >> max_val2;
    palette.get();
    static int num_colors = width2*height2;
    double palette_L[num_colors] = {0.0};
    double palette_a[num_colors] = {0.0};
    double palette_b[num_colors] = {0.0};

    for (int i=0; i<num_colors; i++) {
        unsigned char rgb[3];
        double lab[3];

        rgb[0] = palette.get();
        rgb[1] = palette.get();
        rgb[2] = palette.get();
        srgb_oklab(rgb, lab);

        palette_L[i] = lab[0];
        palette_a[i] = lab[1];
        palette_b[i] = lab[2];
    }
    palette.close();

    system("ffmpeg -loglevel quiet -i input.png dither_input.ppm -y");
    std::ifstream input("dither_input.ppm", std::ios_base::binary);
    char magic_num[3];
    int width = 0, height = 0, max_val = 0;
    input >> magic_num;
    input >> width >> height >> max_val;
    input.get();
    int num_pixels = width*height;

    std::ofstream output("dither_output.ppm", std::ios_base::binary);
    output << magic_num << "\n" << width << " " << height << "\n" << max_val << std::endl;

    double weights[error_length];
    for (int i=0; i<error_length; i++) {
        weights[i] = 1.0/(i+1);
    }

    double errors_L[error_length];
    double errors_a[error_length];
    double errors_b[error_length];
    for (int i=0; i<error_length; i++) {
        errors_L[i] = 0.0;
        errors_a[i] = 0.0;
        errors_b[i] = 0.0;
    }

    for (int i=0; i<num_pixels; i++) {
        unsigned char old_rgb[3];
        double old_lab[3];

        old_rgb[0] = input.get();
        old_rgb[1] = input.get();
        old_rgb[2] = input.get();

        srgb_oklab(old_rgb, old_lab);

        double lab0 = old_lab[0];
        double lab1 = old_lab[1];
        double lab2 = old_lab[2];
        old_lab[0] = l_offset + (lab0-0.5)*l_scale+0.5;
        old_lab[1] = a_offset + (lab1*std::cos(hue_phase*TAU) - lab2*std::sin(hue_phase*TAU))*hue_scale;
        old_lab[2] = b_offset + (lab2*std::cos(hue_phase*TAU) + lab1*std::sin(hue_phase*TAU))*hue_scale;
        lab0 = old_lab[0];
        lab1 = old_lab[1];
        lab2 = old_lab[2];

        double error_lab[3] = {0, 0, 0};
        for (int m=0; m<error_length; m++) {
                error_lab[0] += errors_L[m]*weights[m];
                error_lab[1] += errors_a[m]*weights[m];
                error_lab[2] += errors_b[m]*weights[m];
        }
        old_lab[0] -= error_lab[0]*dither_intensity;
        old_lab[1] -= error_lab[1]*dither_intensity;
        old_lab[2] -= error_lab[2]*dither_intensity;

        double delta[num_colors];
        for (int d=0; d<num_colors; d++){
            double p_lab[3];
            p_lab[0] = palette_L[d];
            p_lab[1] = palette_a[d];
            p_lab[2] = palette_b[d];

            delta[d] = hypot(old_lab, p_lab);
        }
        int amin_delta = 0;
        for (int d=0; d<num_colors; d++){
            if(delta[d] < delta[amin_delta]) {
                amin_delta = d;
            }
        }

        double new_lab[3];
        unsigned char new_rgb[3];
        new_lab[0] = palette_L[amin_delta];
        new_lab[1] = palette_a[amin_delta];
        new_lab[2] = palette_b[amin_delta];

        oklab_srgb(new_lab, new_rgb);

        output << new_rgb[0] << new_rgb[1] << new_rgb[2];

        for(int m=error_length-1; m>0; m--) {
            errors_L[m] = errors_L[m-1];
            errors_a[m] = errors_a[m-1];
            errors_b[m] = errors_b[m-1];
        }
        errors_L[0] = new_lab[0] - lab0;
        errors_a[0] = new_lab[1] - lab1;
        errors_b[0] = new_lab[2] - lab2;
    }

    input.close();
    output.close();
    system("ffmpeg -loglevel quiet -i dither_output.ppm output.png -y && del *.ppm");
    return 0;
}