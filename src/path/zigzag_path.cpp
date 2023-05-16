
#include "zigzag_path.h"

ZigZagPath::ZigZagPath() {
    direction = up_right;
}

void ZigZagPath::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {
    if(!this->ended(rectangle, currentOffset)) {

        int y = rectangle->getY(currentOffset);
        int x = rectangle->getX(currentOffset);
        if ((direction == up_right) && (x==rectangle->width-1)) {
            y+=1;
            direction = down_left;
        } else if((direction == up_right) && (y==0)) {
            x+=1;
            direction = down_left;
        } else if((direction == down_left) && (y==rectangle->height-1)) {
            x+=1;
            direction = up_right;
        } else if ((direction == down_left) && (x==0)) {
            y+=1;
            direction = up_right;
        } else {
            if(direction == up_right) {
                y -= 1;
                x += 1;
            } else {
                y += 1;
                x -= 1;
            }
        }
        currentOffset = rectangle->getOffset(x, y);
    }
}


