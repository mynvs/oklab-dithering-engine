#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#ifndef FLOAT_RECTANGLE_H
#define FLOAT_RECTANGLE_H 1

struct Padding {
    Padding() { top=0; bottom=0; left=0; right=0; }
    int top;
    int bottom;
    int left;
    int right;
};

class FloatRectangle {
    public:
        unsigned width;
        unsigned height;


        FloatRectangle(unsigned width, unsigned height);

        FloatRectangle(FloatRectangle& other, Padding padding);
       
        unsigned getX(unsigned offset) {
            return offset % width;
        }

        unsigned getY(unsigned offset) {
            return offset / width;
        }

        unsigned getOffset(unsigned x, unsigned y) {
            return y*width + x;
        }

        unsigned getPixels() {
            return width*height;
        }

         float get(unsigned offset) {
            assert(offset < this->getPixels());
            return (*buffer)[offset];
        }

        float &getForModification(unsigned offset) {
            assert(offset < this->getPixels());
            return (*buffer)[offset];
        }

        float *get_ptr(unsigned offset) {
            assert(offset < this->getPixels());
            return &((*buffer)[offset]);
        }

        void set(unsigned offset, float value) {
            assert(offset < this->getPixels());
            (*buffer)[offset] = value;
        }

        float get(unsigned x, unsigned y) {
            return get(getOffset(x, y));
        }

        void set(unsigned x, unsigned y, float value) {
            set(getOffset(x, y), value);
        }
     
        bool isPadded() {
            return paddingStack.size() > 0;
        }

        void depad();

    protected:
        std::vector<Padding> paddingStack;
        std::unique_ptr<std::vector<float>> buffer;
};

#endif

