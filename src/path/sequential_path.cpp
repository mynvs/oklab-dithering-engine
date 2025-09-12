
#include "sequential_path.h"

/* Goes back and forth row by row Boustrophedon style */
void BidiSequentialPath::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {
    if(!this->ended(rectangle, currentOffset)) {
        int row = rectangle->getY(currentOffset);
        if ((row % 2) == 0) {
            currentOffset++;
            int new_row = rectangle->getY(currentOffset);
            if (new_row != row) {
                currentOffset += rectangle->width-1;
            }
        } else {
            currentOffset--;
            int new_row = rectangle->getY(currentOffset);
            if (new_row != row) {
                currentOffset += rectangle->width+1;
            }
        }
    }
}

