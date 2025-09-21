#include <limits.h>
#include <float.h>
#include <string.h>
#include "dither.h"
#include "canvas.h"
#include "../utils/fileutils.h"

#define ERROR_LENGTH 25 //From 7

void set_dither_defaults(dither_settings* settings) {
    settings->stalg = errordiffuse;
    settings->dither_intensity = 0.75;
}

typedef struct {
    float lum;
    int idx;
} candidate_t; // BAYER CANDIDATES

static int compare_luminance(const void* a, const void* b) {
    const candidate_t* ca = (const candidate_t*)a;
    const candidate_t* cb = (const candidate_t*)b;
    return (ca->lum > cb->lum) - (ca->lum < cb->lum);
} // BAYER COMPARE

int closest_palette_color(const canvasf_t* canvas, const oklab_settings* oklab,
                              float lab_color[3]) {
    int best_index = 0;
    float best_distance = FLT_MAX;

    for (int c = 0; c < canvas->colors; c++) {
        int palette_idx = c * 3;
        float palette_color[3] = {
            canvas->palette[palette_idx],
            canvas->palette[palette_idx+1],
            canvas->palette[palette_idx+2]
        };

        float distance = hcl_oklab_distance(oklab, lab_color, palette_color);
        if (distance < best_distance) {
            best_distance = distance;
            best_index = c;
        }
    }

    return best_index;
}

void dither(const dither_settings* settings, const oklab_settings* oklab, canvas_t* canvas) {
    canvasf_t lab_canvas = oklab_canvas(canvas);
    if (!lab_canvas.input || !lab_canvas.palette || !lab_canvas.output) {
        return;
    }

    int num_pixels = canvas->width * canvas->height;

    switch(settings->stalg){
        case errordiffuse:{
            const float negative_dither_intensity = -settings->dither_intensity;

            float weights[ERROR_LENGTH];
            for (int i = 0; i < ERROR_LENGTH; i++) {
                weights[i] = 1.0f / (i + 1) * negative_dither_intensity;
            }

            float errors_L[ERROR_LENGTH] = {0};
            float errors_a[ERROR_LENGTH] = {0};
            float errors_b[ERROR_LENGTH] = {0};

            for (int i = 0; i < num_pixels; i++) {
                int p = i * lab_canvas.channels;
                float lab[3] = {
                    lab_canvas.input[p],
                    lab_canvas.input[p+1],
                    lab_canvas.input[p+2]
                };
                oklab_hue_shift(oklab, lab);
                float old_lab[3] = {lab[0], lab[1], lab[2]};
                for (int m = 0; m < ERROR_LENGTH; m++) {
                    lab[0] += errors_L[m] * weights[m];
                    lab[1] += errors_a[m] * weights[m];
                    lab[2] += errors_b[m] * weights[m];
                }
                int bestpindex = closest_palette_color(&lab_canvas, oklab, lab) * 3;
                float bestcolor[3] = {
                    lab_canvas.palette[bestpindex],
                    lab_canvas.palette[bestpindex+1],
                    lab_canvas.palette[bestpindex+2]
                };

                lab_canvas.output[p] = bestcolor[0];
                lab_canvas.output[p+1] = bestcolor[1];
                lab_canvas.output[p+2] = bestcolor[2];
                lab_canvas.output[p+3] = lab_canvas.input[p+3] > 0.5f ? 1 : 0;

                for (int m = ERROR_LENGTH - 1; m > 0; m--) {
                    errors_L[m] = errors_L[m-1];
                    errors_a[m] = errors_a[m-1];
                    errors_b[m] = errors_b[m-1];
                }

                errors_L[0] = bestcolor[0] - old_lab[0];
                errors_a[0] = bestcolor[1] - old_lab[1];
                errors_b[0] = bestcolor[2] - old_lab[2];
            }
            break;
        }
        case ordered:{
            const float intensity = settings->dither_intensity;

            const int matrix_size = 4;
            const int num_candidates = matrix_size * matrix_size;
            static const int bayermatrix[4][4] = {
                { 0,  8,  2, 10},
                {12,  4, 14,  6},
                { 3, 11,  1,  9},
                {15,  7, 13,  5}
            };

            for (int i = 0; i < num_pixels; i++) {
                int x = i % canvas->width;
                int y = i / canvas->width;
                int bx = x % matrix_size;
                int by = y % matrix_size;
                int matrix_idx = bayermatrix[by][bx];

                int p = i * canvas->channels;
                float lab[3] = {
                    lab_canvas.input[p],
                    lab_canvas.input[p + 1],
                    lab_canvas.input[p + 2]
                };
                oklab_hue_shift(oklab, lab);

                candidate_t cand_list[num_candidates]; // LORD HAVE MERCY THIS IS AWFUL
                float goal[3] = {lab[0], lab[1], lab[2]};
                for (int n = 0; n < num_candidates; n++) {
                    int idx = closest_palette_color(&lab_canvas, oklab, goal);
                    cand_list[n].idx = idx;
                    cand_list[n].lum = lab_canvas.palette[idx * 3];

                    float pal_color[3] = {
                        lab_canvas.palette[idx * 3],
                        lab_canvas.palette[idx * 3 + 1],
                        lab_canvas.palette[idx * 3 + 2]
                    };
                    goal[0] += (lab[0] - pal_color[0]) * intensity;
                    goal[1] += (lab[1] - pal_color[1]) * intensity;
                    goal[2] += (lab[2] - pal_color[2]) * intensity;
                }

                qsort(cand_list, num_candidates, sizeof(candidate_t), compare_luminance);

                int best_idx = cand_list[matrix_idx].idx;
                int best_pindex = best_idx * 3;
                lab_canvas.output[p] = lab_canvas.palette[best_pindex];
                lab_canvas.output[p + 1] = lab_canvas.palette[best_pindex + 1];
                lab_canvas.output[p + 2] = lab_canvas.palette[best_pindex + 2];
                lab_canvas.output[p + 3] = lab_canvas.input[p + 3] > 0.5f ? 1 : 0;
            }
            break;
        }
        case none:{
            memcpy(lab_canvas.output, lab_canvas.input, num_pixels * canvas->channels * sizeof(float));
            break;
        }
    }

    for (int i = 0; i < canvas->width*canvas->height; i++) { // welp
        int p = i * canvas->channels;
        float target_color[3];
        oklab_srgb(&lab_canvas.output[p], target_color);
        float_triple_to_bytes(target_color, &canvas->output[p]);
        canvas->output[p+3] = cclamp(lab_canvas.output[p+3] * 255.0f);
    }
    canvasf_clear(&lab_canvas);
}
