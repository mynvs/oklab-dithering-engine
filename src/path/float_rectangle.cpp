#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <cstdio>
#include "float_rectangle.h"

#if __cplusplus < 201703L
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif


FloatRectangle::FloatRectangle(unsigned width, unsigned height) {
    this->width = width;
    this->height = height;
    buffer = std::make_unique<std::vector<float>>();
    buffer->insert(buffer->begin(), width*height, 0.0);
    paddingStack.clear();
}

FloatRectangle::FloatRectangle(FloatRectangle& other, Padding padding) {
    buffer = std::make_unique<std::vector<float>>();
    width = other.width + padding.left + padding.right;
    height = other.height + padding.top + padding.bottom;
    buffer->insert(buffer->begin(), width*height, 0.0);
    paddingStack = other.paddingStack;
    paddingStack.push_back(padding);

    for(int rowIndex=0; rowIndex < other.height; ++rowIndex) {
        unsigned source = other.getOffset(0, rowIndex);
        unsigned destination = this->getOffset(padding.left, rowIndex + padding.top);
        for(int x=0; x< other.width; ++x) {
            this->set(destination+x, other.get(source+x));
        }
    }
}

void FloatRectangle::depad() {
    if (this->isPadded()) {
        Padding padding = paddingStack.back();
        paddingStack.pop_back();
        unsigned newWidth = width - padding.left - padding.right;
        unsigned newHeight = height - padding.top - padding.bottom;
        std::unique_ptr<std::vector<float>> newBuffer = std::make_unique<std::vector<float>>();
        newBuffer->insert(newBuffer->begin(), newWidth*newHeight, 0.0);

        unsigned offset = 0;
        for(unsigned row=0+padding.top; row < height-padding.bottom; ++row) {
            for(unsigned column=0+padding.left; column < width-padding.right; ++column) {
                (*newBuffer)[offset++] = this->get(column, row);
            }
        }
        width = newWidth;
        height = newHeight;
        buffer = std::move(newBuffer);
    }
}

