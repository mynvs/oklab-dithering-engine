#include "../canvas.h"
#include "../dither.h"

#define ERROR_LENGTH 25 //From 7

void errdiff_dither(const dither_settings* settings, const oklab_settings* oklab, const canvasf_t* canvas, const size_t pixelcount);
