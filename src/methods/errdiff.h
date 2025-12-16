#include "../canvas.h"
#include "../dither.h"

#define ERROR_LENGTH 25 //From 7

static void errdiff_dither(const dither_settings* settings, const oklab_settings* oklab, const canvasf_t* canvas, const size_t pixelcount) {
    const float negative_dither_intensity = -settings->dither_intensity;

    float weights[ERROR_LENGTH];
    for (int i = 0; i < ERROR_LENGTH; i++) {
        weights[i] = 1.0f / (i + 1) * negative_dither_intensity;
    }

    float errors_L[ERROR_LENGTH] = {0};
    float errors_a[ERROR_LENGTH] = {0};
    float errors_b[ERROR_LENGTH] = {0};

    for (int i = 0; i < pixelcount; i++) {
        int p = i * canvas->channels;
        float lab[3] = {
            canvas->input[p],
            canvas->input[p+1],
            canvas->input[p+2]
        };
        oklab_hue_shift(oklab, lab);
        float old_lab[3] = {lab[0], lab[1], lab[2]};
        for (int m = 0; m < ERROR_LENGTH; m++) {
            lab[0] += errors_L[m] * weights[m];
            lab[1] += errors_a[m] * weights[m];
            lab[2] += errors_b[m] * weights[m];
        }
        int bestpindex = closest_palette_color(canvas, oklab, lab) * 3;
        float bestcolor[3] = {
            canvas->palette[bestpindex],
            canvas->palette[bestpindex+1],
            canvas->palette[bestpindex+2]
        };

        canvas->output[p] = bestcolor[0];
        canvas->output[p+1] = bestcolor[1];
        canvas->output[p+2] = bestcolor[2];
        canvas->output[p+3] = canvas->input[p+3] > 0.5f ? 1 : 0;

        for (int m = ERROR_LENGTH - 1; m > 0; m--) {
            errors_L[m] = errors_L[m-1];
            errors_a[m] = errors_a[m-1];
            errors_b[m] = errors_b[m-1];
        }

        errors_L[0] = bestcolor[0] - old_lab[0];
        errors_a[0] = bestcolor[1] - old_lab[1];
        errors_b[0] = bestcolor[2] - old_lab[2];
    }
}
