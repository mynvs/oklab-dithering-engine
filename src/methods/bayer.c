#include <stdlib.h>
#include "../canvas.h"
#include "../dither.h"
#include "bayer.h"

void bayer_dither(const dither_settings* settings, const oklab_settings* oklab, const canvasf_t* canvas, const size_t pixelcount){
    const float intensity = settings->dither_intensity;

    const int matrix_size = 4;
    const int num_candidates = matrix_size * matrix_size;
    static const int bayermatrix[4][4] = {
        { 0,  8,  2, 10},
        {12,  4, 14,  6},
        { 3, 11,  1,  9},
        {15,  7, 13,  5}
    };

    candidate_t clist[num_candidates];
    for (int i = 0; i < pixelcount; i++) {
        int matrix_idx = bayermatrix
            [(i % canvas->width) % matrix_size] // x
            [(i / canvas->width) % matrix_size]; // y

        int p = i * canvas->channels;
        float lab[3] = {
            canvas->input[p],
            canvas->input[p + 1],
            canvas->input[p + 2]
        };
        oklab_hue_shift(oklab, lab);

        float goal[3] = {lab[0], lab[1], lab[2]};
        for (int n = 0; n < num_candidates; n++) {
            int idx = closest_palette_color(canvas, oklab, goal);
            clist[n].idx = idx;
            clist[n].lum = canvas->palette[idx * 3];

            float pcol[3] = {
                canvas->palette[idx * 3],
                canvas->palette[idx * 3 + 1],
                canvas->palette[idx * 3 + 2]
            };
            goal[0] += (lab[0] - pcol[0]) * intensity;
            goal[1] += (lab[1] - pcol[1]) * intensity;
            goal[2] += (lab[2] - pcol[2]) * intensity;
        }

        qsort(clist, num_candidates, sizeof(candidate_t), compare_luminance);

        int best_pindex = clist[matrix_idx].idx * 3;
        canvas->output[p] = canvas->palette[best_pindex];
        canvas->output[p + 1] = canvas->palette[best_pindex + 1];
        canvas->output[p + 2] = canvas->palette[best_pindex + 2];
        canvas->output[p + 3] = canvas->input[p + 3] > 0.5f ? 1 : 0;
    }
}
