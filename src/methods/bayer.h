#include "../canvas.h"
#include "../dither.h"

typedef struct {
    float lum;
    int idx;
} candidate_t; // BAYER CANDIDATES

static int compare_luminance(const void* a, const void* b){
    const candidate_t* ca = (const candidate_t*)a;
    const candidate_t* cb = (const candidate_t*)b;
    return (ca->lum > cb->lum) - (ca->lum < cb->lum);
} // sort handling

void bayer_dither(const dither_settings* settings, const oklab_settings* oklab, const canvasf_t* canvas, const size_t pixelcount);
