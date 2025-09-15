#include <cassert>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>
#include "float_rectangle.h"

FloatRectangle::FloatRectangle(unsigned width, unsigned height) {
    this->width = width;
    this->height = height;
    buffer = new float[width*height];
    std::memset(buffer, 0, (width*height) * 4);
    paddingStack.clear();
}

FloatRectangle::FloatRectangle(FloatRectangle& other, Padding padding) {
    width = other.width + padding.left + padding.right;
    height = other.height + padding.top + padding.bottom;
    buffer = new float[width*height];
    std::memset(buffer, 0, (width*height) * 4);
    paddingStack = other.paddingStack;
    paddingStack.push_back(padding);

    for (unsigned r = 0; r < other.height; ++r) {
        float* src = other.buffer + size_t(r) * other.width;
        float* dst = buffer + size_t(r + padding.top) * width + padding.left;
        std::memcpy(dst, src, size_t(other.width) * sizeof(float));
    }
}

void FloatRectangle::depad() {
    if (this->isPadded()) {
        Padding padding = paddingStack.back();
        paddingStack.pop_back();
        unsigned newWidth = width - padding.left - padding.right;
        unsigned newHeight = height - padding.top - padding.bottom;
        float* newBuffer = new float[width*height];

        for (unsigned r = 0; r < newHeight; ++r) {
            float* src = buffer + size_t(r + padding.top) * width + padding.left;
            float* dst = newBuffer + size_t(r) * newWidth;
            std::memcpy(dst, src, size_t(newWidth) * sizeof(float));
        }

        delete[] buffer;
        buffer = newBuffer;
        width = newWidth;
        height = newHeight;
    }
}
