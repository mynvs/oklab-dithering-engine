
#include "sequential_path.h"

/* Goes back and forth row by row Boustrophedon style */
void BidiSequentialPath::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {
    if(!this->ended(rectangle, currentOffset)) {
        unsigned int row = rectangle->getY(currentOffset);
        if ((row % 2) == 0) {
            currentOffset++;
            if (rectangle->getY(currentOffset) != row) {
                currentOffset += rectangle->width-1;
            }
        } else {
            currentOffset--;
            if (rectangle->getY(currentOffset) != row) {
                currentOffset += rectangle->width+1;
            }
        }
    }
}
