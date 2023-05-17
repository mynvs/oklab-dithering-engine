#ifndef SEQUENTIAL_PATH_H
#define SEQUENTIAL_PATH_H 1

#include "path.h"

class SequentialPath: public Path {
    public:
        using Path::Path; //Inherit constructor

        std::unique_ptr<Path> clone() const {
            return std::make_unique<SequentialPath>();
        }
};

/* Goes all the way through all columns in row, then for next row goes other direction, alternating directions
 * each row, Boustrophedon style.
 */
class BidiSequentialPath: public Path {
    public:
        using Path::Path; //Inherit constructor
        void advance(std::shared_ptr<FloatRectangle> rectangle, int &current_offset) override;

        std::unique_ptr<Path> clone() const {
            return std::make_unique<BidiSequentialPath>();
        }
};

#endif
