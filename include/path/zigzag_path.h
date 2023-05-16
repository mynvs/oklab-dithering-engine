#ifndef ZIGZAG_PATH_H
#define ZIGZAG_PATH_H

#include "path.h"

class ZigZagPath: public Path {
    protected:
        enum { up_right, down_left } direction;

    public:
        ZigZagPath();

        void advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) override;

        std::unique_ptr<Path> clone() {
            auto returnValue = std::make_unique<ZigZagPath>();
            returnValue->direction = direction;
            return returnValue;
        }
};     
#endif

